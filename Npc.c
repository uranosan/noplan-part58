#include "Npc.h"

/*-----------------------------------------------------------------------------
  Npc: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static int ActionMove	(Human *human, int key);
static int ActionRotate(Human *human, int key);

/*-----------------------------------------------------------------------------
  Npc: Extern Functions
  ---------------------------------------------------------------------------*/
/* NPCの初期化:
 * @param	*human		初期化するHuman型変数のポインタ
 * @param	*name		名前
 * @param	gid			グループＩＤ
 * @param	y			初期座標Ｙ
 * @param	x			初期座標Ｘ
 * @param	*parrec		パーツレコード
 * @param	parrecSize	パーツレコードのサイズ
 */
extern int NpcInit(Human *human, const char *name, int gid, int y, int x, PartsRecord *parrec, int parrecSize)
{
	/* HUMAN型変数を初期化 */
	int res = HumanInit( human, name, gid, y, x, parrec, parrecSize );
	return res;
}

/* NPCの状態を更新 */
extern int NpcUpdate(Human *human)
{
    return HumanUpdate(human);
}

/* 描画領域内に居るNPCを裏画面に描画 */
extern int NpcPrint(Human *human)
{
	Position printPos = {-1,-1};
	
	for (int i=0; i<NUM_HUMAN_HEIGHT * NUM_HUMAN_WIDTH; i++)
    {
    	int partsIndex = human->partsOrigin + i;// 基点＋i番目のパーツのインデックス
    	
    	if (HumanCanPrintParts(human, &printPos, partsIndex))// パーツを描画できるなら
    	{
   			human->printPos = printPos;// 描画位置を更新
			// パーツを描画
    		ConsolePrintFormat(
    			printPos.y,
    			printPos.x * 2,
    			human->parts[partsIndex].color.fore,
    			human->parts[partsIndex].color.back,
    			human->parts[partsIndex].str
    		);
    	}
	}
	return 1;
}

/* NPC行動:
 * @param	*human		行動を実行するHuman型変数へのポインタ
 * @param	*actions[]	行動を処理する関数ポインタ型の配列
 *						int (*const 配列名[])(Human *) の形を取ること。
 *						配列末尾にNULLポインタ(番兵)を格納すること。
 *						関数は、行動に成功したら「真」。失敗したら「偽」を返す仕様にすること。
 */
extern int NpcAction(Human *human, int (*const actions[])(Human *human))
{
	static int (*const defActions[])(Human *human) = {
		NULL,
	};
	if (human->state.isDead) {// 死んでるなら
		return 0;// 行動しない
	}
	// *actions[] がNULLなら、*defActions[]を実行する
	if (actions == NULL)
	{
		for (int i=0; defActions[i] != NULL; i++)
			if ( defActions[i](human) )
				return 1;// 行動に成功したら抜ける
	}
	else
	{
		for (int i=0; actions[i] != NULL; i++)
			if ( actions[i](human) )
				return 1;// 行動に成功したら抜ける
	}
	return 0;
}

/* 乱数移動
 * @param	human	乱数移動させるNPCへのポインタ
 */
extern int NpcActionRandomMove(Human *human)
{
	int key = CommonGetRandom(0, 3);
	ActionRotate( human, key );
	ActionMove	( human, key );
	return 1;// 移動に成功
}

/* 追跡移動
 * @ret						追跡に失敗したら偽。成功なら真。
 * @param	*human			追跡移動させるNPCへのポインタ
 * @param	doChaseMinDist	追跡を実行する最小距離
 * @param	doChaseMaxDist	追跡を実行する最大距離
 */
extern int NpcActionChase(Human *human, int doChaseMinDist, int doChaseMaxDist)
{
	int			toY			= 0,
				toX			= 0,
				dist		= 0;
	Direction	dir			= human->state.dir;
	// Position	playerPos	= {};//TODO
	Position	playerPos	= {};//TODO
	const Manager* manager  = ManagerGetInstance();

	// playerPos = PlayerGetPos();// プレイヤーの座標を取得//TODO
	playerPos = manager->player->pos;
	
	dist = (int)CommonGetDistance(human->pos.y, human->pos.x, playerPos.y, playerPos.x);
	if (doChaseMaxDist <= dist || doChaseMinDist >= dist)
	{
		return 0;// 追跡範囲外
	}
	if (! MapTraceFieldValue( human->pos, playerPos ))
	{
		return 0;// 直線座標上にマップ上の障害物あり。追跡に失敗
	}
	
	/* 追跡方向と移動量を決定 */
	if (human->pos.y > playerPos.y)
	{
		toY = -1;
		dir = NORTH;
	}
	else if (human->pos.y < playerPos.y)
	{
		toY =  1;
		dir = SOUTH;
	}
	else if (human->pos.x > playerPos.x)
	{
		toX = -1;
		dir = WEST;
	}
	else if (human->pos.x < playerPos.x)
	{
		toX =  1;
		dir = EAST;
	}
	
	/* 追跡を実行 */
	if (HumanCanRotate( human, dir)) {
		human->state.dir = dir;
	}
	if (HumanCanMove( human, human->pos.y + toY, human->pos.x + toX, human->state.dir))
	{
		if (HumanCollideTo( human, NULL, human->pos.y + toY, human->pos.x + toX, human->state.dir))
		{
			HumanMove( human, human->pos.y + toY, human->pos.x + toX);
			return 1;// 追跡に成功
		}
	}
	return 0;// 追跡に失敗
}

/* 攻撃:
 * @ret						攻撃に失敗したら偽。成功したら真。
 * @param	*human			攻撃を実行するHuman型変数のポインタ
 * @param	dammage			攻撃成功時に与えるダメージ量
 * @param	doAttackMaxDist	攻撃を実行する最大距離
 */
extern int NpcActionAttack(Human *human, int dammage, int doAttackMaxDist)
{
	Human *collided;// 光線が当たった対象へのポインタ
	int dist;// 距離
	
	if ((collided = HumanRaycast(human, &dist, NUM_HUMAN_HEIGHT/2, NUM_HUMAN_WIDTH/2)) != NULL)// 光線を投げて当たったら
	{
		if (collided->gid != human->gid)// グループＩＤが違う相手を
		{
			int attackDist = 4;
			if ((collided->state.dir == EAST || collided->state.dir == WEST) &&
				(human->state.dir    == EAST || human->state.dir    == WEST))
				attackDist = 3;
			
			if (dist <= attackDist)// 最大攻撃距離以下なら
			{
				if ( MapTraceFieldValue(human->pos, collided->pos) )//対象との間のマップ上に障害物がなければ
				{
					// 攻撃対象に、ダメージを与える関数を登録。
					collided->applyDammage.callback = HumanApplyDammage;
					collided->applyDammage.dammage += dammage;
					// 攻撃対象に、ノックバック関数を登録
					collided->knockBack.callback = HumanKnockBack;
					collided->knockBack.dir      = human->state.dir;

					human->state.isAttack = 1;// 攻撃している状態のフラグ
					return 1;// 攻撃に成功
				}
			}
		}
	}
	human->state.isAttack = 0;// 攻撃フラグをリセット
	return 0;// 攻撃に失敗
}

/*-----------------------------------------------------------------------------
  Npc: Static Functions
  ---------------------------------------------------------------------------*/
/* 回転 */
static int ActionRotate(Human *human, int key)
{
	Direction dir = human->state.dir;
	
	switch (key)
	{
		case 0	: dir = NORTH;	break;
		case 1	: dir = EAST;	break;
		case 2	: dir = SOUTH;	break;
		case 3	: dir = WEST;	break;
		default	: return 0;		break;
	}
	// 回転できるなら
    if (HumanCanRotate(human, dir))
    {
        human->state.dir = dir;// 回転する
        return 1;
    }
    return 0;
}

/* 移動 */
static int ActionMove(Human *human, int key)
{
	int toY = 0,
		toX = 0;
	
	switch (key)
	{
		case 0	: toY = -1;	break;
		case 1	: toX =  1;	break;
		case 2	: toY =  1; break;
		case 3	: toX = -1; break;
		default	: return 0;	break;
	}
    // 移動できるなら
	if (HumanCanMove(human, human->pos.y + toY, human->pos.x + toX, human->state.dir))
	{
		if (HumanCollideTo(human, NULL, human->pos.y + toY, human->pos.x + toX, human->state.dir))
		{
			HumanMove(human, human->pos.y + toY, human->pos.x + toX);// 移動する
			return 1;
		}
    }
    return 0;
}

