#include "Zombie.h"

/*-----------------------------------------------------------------------------
  Zombie: Static Variables
  ---------------------------------------------------------------------------*/
static int		m_doAttackMaxDist	= 4;	// 攻撃を実行する最大距離
static int		m_dammage			= 10;	// 攻撃ダメージ量
static int		m_doChaseMinDist	= 0;	// 追跡を実行する最小距離
static int		m_doChaseMaxDist	= 10;	// 追跡を実行する最大距離

/*-----------------------------------------------------------------------------
  Zombie: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static int		ActionAttack		(Human *zombie);
static int		ActionRandomMove	(Human *zombie);
static int		ActionChase			(Human *zombie);

/*-----------------------------------------------------------------------------
  Zombie: Extern Functions
  ---------------------------------------------------------------------------*/
/* ゾンビの初期化 */
extern int ZombieInit(Human* zombie)
{
	PartsRecord parrec[] = {
		// str				value		colorFore				colorBack			colorForeIsAlpha colorBackIsAlpha
		// UPPER
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LGRE,0   ,0   ,LGRE},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// UPPER ATTACK
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LGRE},{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""	},{1,1,1,0},{LGRE,0   ,0   ,0	},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT
		{	{"",  "　","| ",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "●","　",""  },{0,1,1,0},{0   ,LGRE,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// RIGHT ATTACK
		{	{"",  "　","| ",""  },{0,1,1,0},{0   ,0   ,DBLA,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　","●"},{0,1,1,1},{0   ,0   ,0   ,LGRE},{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN
		{	{"",  "| "," |",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　","●"},{1,1,1,1},{LGRE,0   ,0   ,LGRE},{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// DOWN ATTACK
		{	{"",  "| "," |",""  },{0,1,1,0},{0   ,DBLA,DBLA,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"　","　","　","　"},{1,1,1,1},{0   ,0   ,0   ,0   },{LCYA,LCYA,LCYA,LCYA},{0,0,0,0},{0,0,0,0}, },
		{	{""  ,"●","　","●"},{0,1,1,1},{0   ,LGRE,0   ,LGRE},{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,1}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT
		{	{"",  " |","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LGRE,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
		// LEFT ATTACK
		{	{"",  " |","　",""  },{0,1,1,0},{0   ,DBLA,0   ,0   },{0   ,LGRE,LGRE,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"●","　","　",""  },{1,1,1,0},{LGRE,0   ,0   ,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{1,0,0,0}, },
		{	{"",  "　","●",""  },{0,1,1,0},{0   ,0   ,LGRE,0   },{0   ,LCYA,LCYA,0   },{0,0,0,0},{0,0,0,0}, },
		{	{"",  "　","　",""  },{0,1,1,0},{0   ,0   ,0   ,0   },{0   ,LRED,LRED,0   },{0,0,0,0},{0,0,0,0}, },
	};
	char name[64]={};
	Position pos = {1,1};

	snprintf(name, sizeof(name), "ゾンビ %d", 1);// 名前の作成
	/* Human型変数を初期化 */
	int res = NpcInit(zombie, name, 1, pos.y, pos.x, parrec, NELEMS(parrec));
	zombie->Init	= ZombieInit;
	zombie->Action	= ZombieAction;
	zombie->Update	= ZombieUpdate;
	zombie->Print	= ZombiePrint;
	return res;
}

/* ゾンビの状態を更新 */
extern int ZombieUpdate(Human* zombie)
{
	return NpcUpdate(zombie);
}

/* 描画領域内に居るゾンビを裏画面に描画 */
extern int ZombiePrint(Human* zombie)
{
	return NpcPrint(zombie);
}

/* ゾンビ行動。*/
extern int ZombieAction(Human* zombie)
{
	static int (*const actions[])(Human *human) = {
		ActionAttack,
		ActionChase,
		ActionRandomMove,
		NULL,	
	};
	return NpcAction(zombie, actions);
}

/*-----------------------------------------------------------------------------
  Zombie: Static Functions
  ---------------------------------------------------------------------------*/

/* 乱数移動
 * @param	zombie	乱数移動させるゾンビへのポインタ
 */
static int ActionRandomMove(Human *zombie)
{
	return NpcActionRandomMove(zombie);
}

/* 追跡移動
 * @param	zombie	追跡移動させるゾンビへのポインタ
 */
static int ActionChase(Human *zombie)
{
	return NpcActionChase(zombie, m_doChaseMinDist, m_doChaseMaxDist);
}

/* 攻撃 */
static int ActionAttack(Human *zombie)
{
	return NpcActionAttack(zombie, m_dammage, m_doAttackMaxDist);
}

