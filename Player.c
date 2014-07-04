#include "Player.h"

static const bool debug = false;

/*-----------------------------------------------------------------------------
  Player: Static Variables
  ---------------------------------------------------------------------------*/
// static int m_doAttackMaxDist = 4;	// 攻撃を実行する最大距離
static int m_dammage = 50;			// 攻撃で与えるダメージ量

/*-----------------------------------------------------------------------------
  Player: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static int ActionMove		(Human* player);
static int ActionRotate		(Human* player);
static int ActionAttack		(Human* player);
static int ActionGrabMove	(Human* player);

/*-----------------------------------------------------------------------------
  Player: Extern Functions
  ---------------------------------------------------------------------------*/
/* プレイヤーの初期化 */
extern int PlayerInit(Human* player)
{
	PartsRecord parrec[] = {
		// str				value		colorFore				colorBack			colorForeIsAlpha colorBackIsAlpha
		// NORTH
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LWHI,0   ,0   ,LWHI},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// NORTH ATTACK
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LWHI},{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""	},{1,1,1,0},{LWHI,0   ,0   ,0	},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT
		{	{"",  "　","・",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "●","　",""  },{0,1,1,0},{0   ,LWHI,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT ATTACK
		{	{"",  "　","・",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LWHI},{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN
		{	{"",  "・","・",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LWHI,0   ,0   ,LWHI},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN ATTACK
		{	{"",  "・","・",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{""  ,"●","　","●"},{0,1,1,1},{0   ,LWHI,0   ,LWHI},{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT
		{	{"",  "・","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LWHI,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT ATTACK
		{	{"",  "・","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""  },{1,1,1,0},{LWHI,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LWHI,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
	};
	
	/* Human型変数を初期化 */
	HumanInit(player, "ゆっくり", 0, 5, 5, parrec, NELEMS(parrec));
	
	player->health = NUM_PLAYER_HEALTH;// 体力
	player->pos = MapGetStartSpawnPos();// ゲームスタート地点
	// 関数の登録。
	player->Init = PlayerInit;
	player->Action = PlayerAction;
	player->Update = PlayerUpdate;
	player->Print = PlayerPrint;
	return 1;
}

/* プレイヤーの状態を更新 */
extern int PlayerUpdate(Human* player)
{
	HumanUpdate(player);

	if (player->state.isDead)// 死んでいるなら、
	{
		/*if (debug) */ConsolePushStrStack("%s は死亡した\nGAME　OVER", player->name);
	}
	return 1;
}

/* プレイヤーを裏画面に描画 */
extern int PlayerPrint(Human* player)
{
	HumanPrint(player, player->printPos.y, player->printPos.x);

	// debug
	if (debug)
	{
		ConsolePushStrStack("Player pos: %2d %2d", player->pos.y, player->pos.x );
		for (int i = 0; i < NELEMS(player->breadcrumbs); i++)
		{
			ConsolePushStrStack("%02d: %2d %2d",
				i, player->breadcrumbs[i].y, player->breadcrumbs[i].x );
		}
	}
	return 1;
}

/* プレイヤーの行動エントリー。キー入力に応じて処理する */
extern int PlayerAction(Human* player)
{
	ActionRotate( player );
	ActionMove	( player );
	ActionGrabMove( player );
	ActionAttack( player );
	return 1;
}

/*---------------------------------------------------------------------------
  Player: Static Functions
  -------------------------------------------------------------------------*/

/* 目の前に他キャラクターがいれば、そのキャラを掴んで移動する。
 * いなければ、そのまま移動する。
 * @return 移動に成功したら１．失敗なら０．
*/
static int ActionGrabMove(Human* player)
{
	int key = KeyGet();
	Direction dir;
	Position to = player->pos;

	/* キー入力に応じて、移動量、それに掴んだキャラをノックバックさせる方向を決定 */
	switch (key)
	{
		case NUM_KEY_GRUB_UP:		to.y--; dir = NORTH; break;
		case NUM_KEY_GRUB_RIGHT:	to.x++; dir = EAST; break;
		case NUM_KEY_GRUB_DOWN:		to.y++; dir = SOUTH; break;
		case NUM_KEY_GRUB_LEFT:		to.x--; dir = WEST; break;
		default: return 0; break;
	}

	Human *collided;// 掴む対象。
	int	dist;// その対象までの距離。
	if ((collided = HumanRaycast(player, &dist, NUM_HUMAN_HEIGHT/2, NUM_HUMAN_WIDTH/2)) != NULL)// 光線を投げて、誰かに当たって、
	{
		int grabDist = 4;
		if ((collided->state.dir == EAST || collided->state.dir == WEST) &&
			(player->state.dir   == EAST || player->state.dir   == WEST))
			grabDist = 3;

		if (dist <= grabDist)// 掴める範囲内で、
		{
			if ( MapTraceFieldValue(player->pos, collided->pos) )// 自身と対象間のマップ上に障害物がなければ、
			{
				collided->knockBack.callback = HumanKnockBack;// 対象をノックバックさせる。
				collided->knockBack.dir = dir;
			}
		}
	}
	HumanMoveToPos(player, to, false);// 回転せずに移動。
	return 1;
}

/* 回転
　* @return 回転に成功なら１、失敗なら０．
*/
static int ActionRotate(Human* player)
{
	Direction dir = player->state.dir;
	
	int key = KeyGet();
	switch (key)
	{
		case NUM_KEY_UP		: dir = NORTH;	break;
		case NUM_KEY_RIGHT	: dir = EAST;	break;
		case NUM_KEY_DOWN	: dir = SOUTH;	break;
		case NUM_KEY_LEFT	: dir = WEST;	break;
		default				: return 0;	break;
	}
	// 回転できるなら
    if (HumanCanRotate(player, dir))
    {
        player->state.dir = dir;// 回転する
        return 1;
    }
    return 0;
}

/* 移動
　* @return 移動に成功なら１、失敗なら０．
*/
static int ActionMove(Human* player)
{
	int		toY = 0,
			toX = 0;
	Human	*collided = NULL;

	int key = KeyGet();
	switch (key)
	{
		case NUM_KEY_UP		: toY = -1;	break;
		case NUM_KEY_RIGHT	: toX =  1;	break;
		case NUM_KEY_DOWN	: toY =  1; break;
		case NUM_KEY_LEFT	: toX = -1; break;
		default				: return 0;	break;
	}
    if (HumanCanMove(player, player->pos.y + toY, player->pos.x + toX, player->state.dir))// マップ上の障害物に衝突せず、
    {
	    if (HumanCollideTo(player, &collided, player->pos.y + toY, player->pos.x + toX, player->state.dir))// ほかのHumanと衝突しなければ、
		{
			HumanDropBreadCrumb(player);// ブレッドクラムを落とす
			HumanMove(player, player->pos.y + toY, player->pos.x + toX);// 移動する
			return 1;// 移動に成功
		}
		else if(collided != NULL)// Human型変数と衝突
		{
			// 衝突対象のHuman にノックバック関数を登録。
			collided->knockBack.callback = HumanKnockBack;
			collided->knockBack.dir      = player->state.dir;
			return 0;// 移動に失敗
		}
    }
    return 0;// 移動に失敗
}

/* 攻撃
　* @return 攻撃に成功なら１、失敗なら０．
*/
static int ActionAttack(Human* player)
{
	player->state.isAttack = 0;// 攻撃状態をリセット。
	
	if (KeyPressed(NUM_KEY_ATTACK))// 攻撃キーが押下されているなら、
	{
		player->state.isAttack = 1;// 状態を攻撃状態に。
		
		Human *collided;// 光線が当たった対象へのポインタ。
		int	dist;		// その対象までの距離。
		if ((collided = HumanRaycast(player, &dist, NUM_HUMAN_HEIGHT/2, NUM_HUMAN_WIDTH/2)) != NULL)// 光線を投げて、誰かに当たって、
		{
			int attackDist = 4;
			if ((collided->state.dir == EAST || collided->state.dir == WEST) &&
				(player->state.dir   == EAST || player->state.dir   == WEST))
				attackDist = 3;
			
			if (dist <= attackDist)// 攻撃範囲内で、
			{
				if ( MapTraceFieldValue(player->pos, collided->pos) )// 自身と攻撃対象間のマップ上に障害物がなければ、
				{
					// 攻撃する。
					// 攻撃対象のHuman にダメージを与える関数を登録。
					collided->applyDammage.callback = HumanApplyDammage;
					collided->applyDammage.dammage = m_dammage;
					// 攻撃対象のHuman にノックバック関数を登録。
					collided->knockBack.callback = HumanKnockBack;
					collided->knockBack.dir = player->state.dir;
					return 1;// 攻撃に成功
				}
			}
		}
	}
	return 0;// 攻撃に失敗
}

