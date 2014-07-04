#include "Human.h"

/*-----------------------------------------------------------------------------
  Human: Static Functions
  ---------------------------------------------------------------------------*/

/* キャラクターの状態からparts[]の基点添え字を返す。範囲外なら０を返す。
 * @param	dir		キャラクターの向き
 */
static int GetPartsOrigin(int dir, int isAttack)
{
	int offs = (dir * (32));
	offs += (isAttack * 16);
	
	if (offs < 0 || offs > (NUM_HUMAN_PARTS_SIZE - (NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH)))
		return 0;// そんな添え字アカン
	return offs;
}

/*-----------------------------------------------------------------------------
  Human: Extern Functions
  ---------------------------------------------------------------------------*/

/* Human型変数を topos にむけて移動させる。移動には衝突判定、向き変更を伴う */
extern int HumanMoveToPos(Human *human, Position topos, bool doRotate)
{
	Direction dir = human->state.dir;
	Position movepos = human->pos;

	if (topos.y > human->pos.y) {
		movepos.y++;
		dir = SOUTH;
	}
	else if (topos.y < human->pos.y) {
		movepos.y--;
		dir = NORTH;
	}
	else if (topos.x > human->pos.x) {
		movepos.x++;
		dir = EAST;
	}
	else if (topos.x < human->pos.x) {
		movepos.x--;
		dir = WEST;
	}

	if (doRotate && HumanCanRotate(human, dir)) {
		human->state.dir = dir;
	}

	if (HumanCanMove(human, movepos.y, movepos.x, human->state.dir))
	{
		if (HumanCollideTo(human, NULL, movepos.y, movepos.x, human->state.dir))
		{
			HumanMove(human, movepos.y, movepos.x);
			return 1;// 移動に成功
		}
	}
	return 0;// 移動に失敗
}

/* Human型変数を初期化
 * @param	human		初期化するHuman型変数
 * @param	name		名前
 * @param	gid			グループＩＤ
 * @param	y			初期座標Ｙ
 * @param	x			初期座標Ｘ
 * @param	parrec		PartsRecord型配列。パーツの初期化で使用。
 * @param	parrecSize	parrecの要素数
 */
extern int HumanInit(Human *human, const char *name, int gid, int y, int x, PartsRecord *parrec, int parrecSize)
{
	Human emptyHuman = {};
	*human = emptyHuman;

	snprintf(human->name, sizeof(human->name), "%s", name);// 名前
	human->health		= 100;	// 体力
	human->gid			= gid;	// グループＩＤ
	human->pos.y		= y;	// 座標Ｙ
	human->pos.x		= x;	// 座標Ｘ
	human->state.dir	= SOUTH;// 向き
	human->partsOrigin  = 0;

	// パーツの初期化
	int m, n;
	for (int i=0; i<NELEMS(human->parts); i++)
	{
		m = i / NUM_HUMAN_WIDTH;// 添え字を二次元に変換 m を行
		n = i % NUM_HUMAN_WIDTH;// 添え字を二次元に変換 n を列
		if (parrec[ m ].str[ n ][0] != '\0')
		{
			human->parts[ i ].pos.y = i / NUM_HUMAN_WIDTH % NUM_HUMAN_HEIGHT;
			human->parts[ i ].pos.x = i % NUM_HUMAN_WIDTH;
		}
		snprintf(human->parts[ i ].str, sizeof(human->parts[ i ].str), "%s", parrec[ m ].str[ n ]);
		human->parts[ i ].value				= parrec[ m ].value[ n ];
		human->parts[ i ].color.fore		= parrec[ m ].colorFore[ n ];
		human->parts[ i ].color.back		= parrec[ m ].colorBack[ n ];
		human->parts[ i ].color.foreIsAlpha = parrec[ m ].colorForeIsAlpha[ n ];
		human->parts[ i ].color.backIsAlpha = parrec[ m ].colorBackIsAlpha[ n ];
	}
	// ブレッドクラムの初期化
	HumanInitBreadCrumb(human);
	return 1;
}

/* Human型変数の状態を更新
 * @param	human
 */
extern int HumanUpdate(Human *human)
{
	if (human->health <= 0)// 体力が0以下なら
	{
		human->state.isDead = 1;// 死亡
	}
	// パーツの添え字の基点を更新
    human->partsOrigin = GetPartsOrigin(human->state.dir, human->state.isAttack);
    
	// コールバックが登録されているなら実行
	if (human->applyDammage.callback)
	{
		// ダメージを受ける。
		human->applyDammage.callback( human, human->applyDammage.dammage );
		human->applyDammage.dammage  = 0;
		human->applyDammage.callback = NULL;
	}
	if (human->knockBack.callback)
	{
		// ノックバックする。
		human->knockBack.callback( human, human->knockBack.dir );
		human->knockBack.callback = NULL;
	}
	// パーツの透過処理,透過フラグが０でなければ、各パーツにマップの色を適用
	for (int i= 0; i< NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH; i++)
	{
		int index = human->partsOrigin + i;
		int fy = (human->pos.y + human->parts[index].pos.y) % NUM_MAP_HEIGHT;
		int fx = (human->pos.x + human->parts[index].pos.x) % NUM_MAP_WIDTH;
		Colorset mapcolor = MapGetFieldColor( fy, fx );

		if (human->parts[index].color.foreIsAlpha) human->parts[index].color.fore = mapcolor.fore;
		if (human->parts[index].color.backIsAlpha) human->parts[index].color.back = mapcolor.back;
	}
	return 1;
}

/* Human型変数を裏画面に描画
 * @param	*human
 * @param	orgy	描画位置オリジンＹ
 * @param	orgx	描画位置オリジンＸ
 */
extern int HumanPrint(Human * human, int orgy, int orgx)
{
	for (int i=0; i<NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH; i++)
    {
        int index = human->partsOrigin + i;
		if (human->parts[index].str[0] == '\0')
			continue;
		
		ConsoleSetColor(
			human->parts[index].color.fore,
			human->parts[index].color.back
		);
		ConsoleSetCursor(
			orgy + human->parts[index].pos.y,
			orgx * 2 + human->parts[index].pos.x * 2
		);
		ConsolePrint(human->parts[index].str);
	}
	return 1;
}

/* Human型変数のパーツ文字列が、ゲーム画面内に描画できるかを調べる。
 * 描画できる場合、引数のprintPosへ描画位置を格納する。
 * 描画できない場合、引数のprintPosへ-1を格納する。
 *
 * @ret					描画できるなら１。できなければ０
 * @param	*human		パーツを保持するHumanへのアドレス
 * @param	*printPos	描画座標を格納する変数へのアドレス
 * @param	partsIndex	パーツの添え字
 */
extern int HumanCanPrintParts(Human *human, Position *printPos, int partsIndex)
{
	if (partsIndex >= NELEMS(human->parts))
		return 0;// 添え字エラー
	if (human->parts[partsIndex].str[0] == '\0')
		return 0;// 描画する文字列が無い
	// マップの描画座標
	Position mapstart = MapGetPrintStartPos();
	Position mapend = MapGetPrintEndPos();
	// パーツの座標（キャラクターのデータ座標からの相対）
	int partsY = human->pos.y + human->parts[partsIndex].pos.y;
	int partsX = human->pos.x + human->parts[partsIndex].pos.x;
	
	if ((partsY < mapstart.y) || (partsY >= mapend.y) ||
		(partsX < mapstart.x) || (partsX >= mapend.x))
	{
		printPos->y = printPos->x = -1;
		return 0;// 描画できない
	}
	else
	{
		if (partsY > mapstart.y) {
			printPos->y = partsY - mapstart.y;
		} else {
			printPos->y = mapstart.y - partsY;
		}
		if (partsX > mapstart.x) {
			printPos->x = partsX - mapstart.x;
		} else {
			printPos->x = mapstart.x - partsX;
		}
		return 1;// 描画できる
	}
}

/* Human型変数の座標を、引数の座標で更新
 * @param	*human
 * @param	y		更新させる座標Ｙ
 * @param	x		更新させる座標Ｘ
 */
extern int HumanMove(Human * human, int y, int x)
{
	human->pos.y = y;
	human->pos.x = x;
	return 1;
}

/* Human型変数が、引数の座標上に移動できるかを判定
 * @ret		移動可能なら真。できなければ偽。
 * @param	*human	
 * @param	y		
 * @param	x		
 * @param	dir		
 */
extern int HumanCanMove(Human *human, int y, int x, int dir)
{
	/*
		// マップ範囲から出るようなら
		if (y < 0 || y + NUM_HUMAN_HEIGHT >= NUM_MAP_HEIGHT || x < 0 || x + NUM_HUMAN_WIDTH >= NUM_MAP_WIDTH) {
			return 0;// 移動できない
		}
	*/
	// マップ上の衝突物を検出
    int offs = GetPartsOrigin(dir, human->state.isAttack);
    
	for (int i= 0; i< NUM_HUMAN_HEIGHT; i++)
	{
		for (int j= 0; j< NUM_HUMAN_WIDTH; j++)
		{
            int index = offs + (i * NUM_HUMAN_WIDTH + j);
			if ( human->parts[index].value )
			{
				int value = MapGetFieldValue( y+i, x+j );
				if ( MapIsCollisionFieldValue( value ))
					return 0;// 移動できない
			}
		}
	}
	return 1;// 移動できる
}

/* 回転できるかを調べる
 * @ret				回転できるなら１。できなければ０
 * @param	*human	回転させようとしているHumanへのアドレス
 * @param	dir		回転させる方向
 */
extern int HumanCanRotate(Human *human, int dir)
{
	if (! HumanCanMove(human, human->pos.y, human->pos.x, dir))
		return 0;// 回転できない
	if (! HumanCollideTo(human, NULL, human->pos.y, human->pos.x, dir))
		return 0;// 回転できない
	return 1;// 回転できる
}

/* Human型変数同士の衝突判定
 * @ret					衝突ありなら０、なしなら１
 * @param *human		衝突する側
 * @param *collided		衝突時の衝突対象を格納させるポインタのアドレス
 * @param y      		*humanが移動したい座標Y
 * @param y      		*humanが移動したい座標X
 * @param dir			*humanが向きたい方向
 */
extern int HumanCollideTo(Human *human, Human **collided, int y, int x, int dir)
{
	const	Manager* manager = ManagerGetInstance();
	Human	*to;
	int		step;
	int		saY, saX;
	int		i, j, k, index, offs;
	int 	tempHuman[NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH];
	int		tempTo[NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH];

	to = 0;
	for (step = 0; step < manager->humansSize; ++step)
	{
		to = manager->humans[step];// 衝突する対象(Human型変数)へのポインタ
		if (to == human)
			continue;// 自分自身なら処理を飛ばす

		// それぞれの座標の差を求める
		saY = abs(y - to->pos.y);
		saX = abs(x - to->pos.x);

		// 矩形での判定
		if (saY < NUM_HUMAN_HEIGHT && saX < NUM_HUMAN_WIDTH)// 矩形での衝突あり
		{
			offs = GetPartsOrigin( dir, human->state.isAttack);// パーツ添え字基点 
			
			// 判定準備（衝突範囲の抜き出し）
			// 衝突する側
			k=0;
			for (i=saY; i<NUM_HUMAN_HEIGHT; i++)
			{
				for (j=saX; j<NUM_HUMAN_WIDTH; j++)
				{
					index = offs + (i * NUM_HUMAN_WIDTH + j);
					tempHuman[k++] = human->parts[index].value;
				}
			}
			tempHuman[k] = -1;// 番兵 

			// 衝突される側
			k=0;
			for (i=0; i<NUM_HUMAN_HEIGHT - saY; i++)
			{
				for (j=0; j<NUM_HUMAN_WIDTH - saX; j++)
				{
					index = to->partsOrigin + (i * NUM_HUMAN_WIDTH + j);
					tempTo[k++] = to->parts[index].value;
				}
			}
			tempTo[k] = -1;// 番兵 

			// 判定
			for (i=0; tempHuman[i] != -1; i++)
			{
				for (j=0; tempTo[j] != -1; j++)
				{
					if (tempHuman[i] > 0 && tempTo[i] > 0)
					{
						if (collided != NULL)
						{
							*collided = to;// 衝突対象のアドレスを格納
						}
						return 0;//衝突あり
					}
				}
			}
		}
	}
	return 1;//衝突なし
}

/* Human型変数がHuman型変数へ、光線(Ray)を投げる(cast)
 * @ret				光線が当たったとき、当たった対象のアドレス。当たらなければ NULL。
 * @param	*human	光線を投げる本人。
 * @param	*dist	光線が当たったとき、座標の2点間の距離を格納。
 */
extern Human* HumanRaycast(Human *human, int *dist, int height, int width)
{
	Human	*to;
	int		i;
	int		current_dist	= -1;
	int		min_dist		= -1;
	const Manager* manager = ManagerGetInstance();
	Human* nearhuman = NULL;

	for (i = 0; i < manager->humansSize; ++i)
	{
		to = manager->humans[i];
		if (to == human)// 自分自身なら処理を飛ばす
			continue;
		
		switch (human->state.dir)
		{
			case NORTH:// 上を向いているとき、上方向に光線を投げる
				if (human->pos.y >= to->pos.y && abs(to->pos.x - human->pos.x) < width)// 光線が当たった
				{
					current_dist = (int)CommonGetDistance(human->pos.y, human->pos.x, to->pos.y, to->pos.x);// 距離をゲッツ
				}
				break;
			case SOUTH:
				if (to->pos.y >= human->pos.y && abs(to->pos.x - human->pos.x) < width)
				{
					current_dist = (int)CommonGetDistance(human->pos.y, human->pos.x, to->pos.y, to->pos.x);// 距離をゲッツ
				}
				break;
			case EAST:
				if (to->pos.x >= human->pos.x && abs(to->pos.y - human->pos.y) < height)
				{
					current_dist = (int)CommonGetDistance(human->pos.y, human->pos.x, to->pos.y, to->pos.x);// 距離をゲッツ
				}
				break;
			case WEST:
				if (human->pos.x >= to->pos.x && abs(to->pos.y - human->pos.y) < height)
				{
					current_dist = (int)CommonGetDistance(human->pos.y, human->pos.x, to->pos.y, to->pos.x);// 距離をゲッツ
				}
				break;
			default:
				break;
		}
		// 最初の更新
		if (min_dist == -1 && current_dist != -1)
		{
			min_dist  = current_dist;// 光線が誰かに当たって距離を測定したら、最小の距離を更新
			nearhuman = to;// 近くにいる対象を更新。
		}
		// 光線を投げて近くにいる対象との距離と、near human を更新
		if (current_dist < min_dist)
		{
			min_dist  = current_dist;
			nearhuman = to;
		}
	}

	if (nearhuman == NULL)// 光線が当たらなかった
	{
		return NULL;
	}
	else// 光線が当たった
	{
		*dist = min_dist;// 距離を格納
		return nearhuman;// 最寄の光線が当たった対象のアドレスを返す
	}
}

/* humanの体力にダメージを与える
 * @param *human	ダメージを与える対象
 * @param dmg		ダメージ量
 */
extern int HumanApplyDammage (Human *human, int dmg)
{
	human->health -= dmg;// ダメージを受けさせる。

	Position adds[] = {{2,2},{0,2},{2,0},{-2,-2},{0,-2},{-2,0}};
	Position add;

	for (int i= 0; i< 6; ++i)
	{
		add = adds[ CommonGetRandom(0, NELEMS(adds)-1) ];

		EffectPopupMessage(
			"★",
			human->printPos.y + add.y,
			(human->printPos.x + add.x) * 2,
			L_YELLOW,
			BLACK 
		);
	}
	return 1;
}

/* humanをノックバックさせる。
 * @param *human	ノックバックさせる対象
 * @param dirFrom	ノックバックさせる方向
 */
extern int HumanKnockBack(Human *human, Direction dirFrom)
{
	if (human == NULL)
		return 0;
	
	Position amounts[] = { {-1,0}, {0,1}, {1,0}, {0,-1},};
	if (dirFrom >= NELEMS(amounts))
		return 0;
	Position amount = amounts[dirFrom];
	
	if (! HumanCollideTo(human, NULL, human->pos.y + amount.y, human->pos.x + amount.x, human->state.dir))
	{
		return 0;// ノックバック方向に衝突物（human）あり。ノックバック失敗
	}
	
	if (HumanCanMove(human, human->pos.y + amount.y, human->pos.x + amount.x, human->state.dir))
	{// 動けるなら
		HumanMove(human, human->pos.y + amount.y, human->pos.x + amount.x);// 動く
		return 1;// 成功
	}
	return 0;// 失敗
}

/* ブレッドクラムを初期化 */
extern void HumanInitBreadCrumb(Human *human)
{
	for (int i = 0; i < NELEMS(human->breadcrumbs); ++i)
	{
		human->breadcrumbs[i].y = -1;
		human->breadcrumbs[i].x = -1;
	}
}

/* ブレッドクラムを落とす */
extern void HumanDropBreadCrumb(Human *human)
{
	for (int i = NELEMS(human->breadcrumbs) - 1; i > 0; --i)
	{
		human->breadcrumbs[i] = human->breadcrumbs[i - 1];
	}
	human->breadcrumbs[0] = human->pos;
}

/* ブレッドクラムを探す
 * @ret				ブレッドクラムを見つけたら真、その場合 仮引数*foundposにブレッドクラムの座標を格納する。でなければ偽。
 * @param *human	ブレッドクラムを探す人
 * @param *target	ブレッドクラムを落とす人
 * @param *foundpos	ブレッドクラムを発見した場合、その位置を格納。
 */
extern int HumanFindBreadCrumb(Human *human, Human *target, Position *foundpos)
{
	int y = human->pos.y;
	int x = human->pos.x;
	int foundcrumb = -1;

	for (int i = 0; i < NELEMS(target->breadcrumbs); i++)
	{
		if ( (y == target->breadcrumbs[i].y) && (x == target->breadcrumbs[i].x) )
		{
			foundcrumb = i;
			break;
		}
		else if ( (y + 1 == target->breadcrumbs[i].y) && (x == target->breadcrumbs[i].x) )
		{
			foundcrumb = i;
			break;
		}
		else if ( (y == target->breadcrumbs[i].y) && (x + 1 == target->breadcrumbs[i].x) )
		{
			foundcrumb = i;
			break;
		}
		else if ( (y - 1 == target->breadcrumbs[i].y) && (x == target->breadcrumbs[i].x) )
		{
			foundcrumb = i;
			break;
		}
		else if ( (y == target->breadcrumbs[i].y) && (x - 1 == target->breadcrumbs[i].x) )
		{
			foundcrumb = i;
			break;
		}
	}

	if (foundcrumb >= 0)
	{
		*foundpos = target->breadcrumbs[ foundcrumb ];
		return 1;// ブレッドクラムを発見
	}
	return 0;// ブレッドクラムを発見できなかった
}

