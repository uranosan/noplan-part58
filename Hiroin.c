#include "Hiroin.h"

static const bool debug = false;

/*-----------------------------------------------------------------------------
  Hiroin: Static Variables
  ---------------------------------------------------------------------------*/
static int		m_doAttackMaxDist	= 4;	// 攻撃を実行する最大距離
static int		m_dammage			= 20;	// 攻撃ダメージ量
static int		m_doChaseMinDist	= 6;	// 追跡を実行する最小距離
static int		m_doChaseMaxDist	= 14;	// 追跡を実行する最大距離

/*-----------------------------------------------------------------------------
  Hiroin: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static int		ActionAttack	(Human *hiroin);
static int		ActionRandomMove(Human *hiroin);
static int		ActionChase		(Human *hiroin);
static int		ActionChaseBreadCrumb(Human *hiroin);

/*-----------------------------------------------------------------------------
  Hiroin: Extern Functions
  ---------------------------------------------------------------------------*/
/* ヒロインの初期化 */
extern int HiroinInit(Human* hiroin)
{
	PartsRecord parrec[] = {
		// str				value		colorFore				colorBack			colorForeIsAlpha colorBackIsAlpha
		// UPPER
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LPIN,LPIN,LPIN,LPIN},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LWHI,0   ,0   ,LWHI},{0   ,LPIN,LPIN,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// UPPER ATTACK
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LWHI},{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LPIN,LPIN,LPIN,LPIN},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""	},{1,1,1,0},{LWHI,0   ,0   ,0	},{0   ,LPIN,LPIN,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT
		{	{"",  "　","| ",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "●","　",""  },{0,1,1,0},{0   ,LWHI,0   ,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT ATTACK
		{	{"",  "　","| ",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LWHI},{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN
		{	{"",  "| "," |",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LPIN,LPIN,LPIN,LPIN},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LWHI,0   ,0   ,LWHI},{0   ,LPIN,LPIN,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN ATTACK
		{	{"",  "| "," |",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LPIN,LPIN,LPIN,LPIN},{0,0,0,0},{0,0,0,0}, },
		{	{""  ,"●","　","●"},{0,1,1,1},{0   ,LWHI,0   ,LWHI},{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT
		{	{"",  " |","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LWHI,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT ATTACK
		{	{"",  " |","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LWHI,LWHI,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""  },{1,1,1,0},{LWHI,0   ,0   ,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LWHI,0   },{0   ,LPIN,LPIN,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
	};
	Position initPos = {10,1};// 座標
	char name[64] = {};

	snprintf(name, sizeof(name), "ヒロイン %d", 1);// 名前の作成
    NpcInit(hiroin, name, 1, initPos.y, initPos.x, parrec, NELEMS(parrec));// Human型変数を初期化

    hiroin->Init = HiroinInit;
	hiroin->Action = HiroinAction;
	hiroin->Update = HiroinUpdate;
	hiroin->Print = HiroinPrint;
	return 1;
}

/* ヒロインの状態を更新 */
extern int HiroinUpdate(Human* hiroin)
{
	return NpcUpdate(hiroin);
}

/* 描画領域内に居るヒロインを裏画面に描画 */
extern int HiroinPrint(Human* hiroin)
{
	return NpcPrint(hiroin);
}

/* ヒロイン行動。*/
extern int HiroinAction(Human* hiroin)
{
	static int (*const actions[])(Human *human) = {
		// ActionAttack,
		ActionChaseBreadCrumb,
		ActionChase,
		// ActionRandomMove,
		NULL,
	};
	return NpcAction(hiroin, actions);
}

/*-----------------------------------------------------------------------------
  Hiroin: Static Functions
  ---------------------------------------------------------------------------*/

static int ActionChaseBreadCrumb(Human *hiroin)
{
	const Manager* manager = ManagerGetInstance();//TODO
	Position foundpos = {};

	// if (HumanFindBreadCrumb(hiroin, PlayerGetPointer(), &foundpos ))//TODO
	if (HumanFindBreadCrumb(hiroin, manager->player, &foundpos ))
	{
		if (debug) ConsolePushStrStack("Hiroin found breadcrumb: %2d %2d", foundpos.y, foundpos.x );
		return HumanMoveToPos(hiroin, foundpos, true);// 移動に成功したら追跡成功
	}
	return 0;// 追跡に失敗
}

/**
 * 乱数移動
 * @param	hiroin	乱数移動させるヒロインへのポインタ
 */
static int ActionRandomMove(Human *hiroin)
{
	return NpcActionRandomMove(hiroin);
}

/**
 * 追跡移動
 * @param	hiroin	追跡移動させるヒロインへのポインタ
 */
static int ActionChase(Human *hiroin)
{
	return NpcActionChase(hiroin, m_doChaseMinDist, m_doChaseMaxDist);
}

/* 攻撃 */
static int ActionAttack(Human *hiroin)
{
	return NpcActionAttack(hiroin, m_dammage, m_doAttackMaxDist);
}

