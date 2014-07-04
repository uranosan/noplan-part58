#ifndef __HUMAN_H_INCLUDED__
#define __HUMAN_H_INCLUDED__

#include <stdlib.h>

#include "Manager.h"

#include "Common.h"
#include "Console.h"
#include "Map.h"
#include "Effect.h"

/*-----------------------------------------------------------------------------
  Human: Global Numbers
  ---------------------------------------------------------------------------*/

enum TAG_NUM_HUMAN {
	NUM_HUMAN_HEIGHT		= 4,			/* 高さ */
	NUM_HUMAN_WIDTH			= 4,			/* 幅 */
	NUM_HUMAN_DIR_MAX		= 4,			/* 向き（四方向）の数 */
	NUM_HUMAN_ANIME_ATTACK	= 4,			/* 攻撃アニメの数 */
	NUM_HUMAN_PARTS_SIZE	= (16*4 + 16*4),/* human.parts[] の最大要素数 */
	NUM_HUMAN_ARRAY_MAX		= 100,			/* m_humanarray[]の最大要素数 */
	NUM_HUMAN_NAME_MAX		= 32,			/* human.name[]の最大要素数 */
	NUM_HUMAN_MAX_BREADCRUMBS = 24,			/* ブレッドクラム経路の最大要素数 */
};

/*-----------------------------------------------------------------------------
  Human: Global Typedef Structers
  ---------------------------------------------------------------------------*/

enum Direction;
typedef enum Direction Direction;

struct Parts;
typedef struct Parts Parts;
struct PartsRecord;
typedef struct PartsRecord PartsRecord;
struct Human;
typedef struct Human Human;
struct State;
typedef struct State State;

struct Parts {
	char		str[3];	// パーツの描画に用いる文字列
	int		 	value;	// パーツが持つ数値（主に衝突判定で使用）
	Position	pos;	// パーツの座標。キャラクター座標からの相対座標
	Colorset	color;	// パーツの描画時に用いる色数値と透過フラグ
};

struct PartsRecord {
	char str			[NUM_HUMAN_WIDTH][3];
	int value			[NUM_HUMAN_WIDTH];
	int colorFore		[NUM_HUMAN_WIDTH];
	int colorBack		[NUM_HUMAN_WIDTH];
	int colorForeIsAlpha[NUM_HUMAN_WIDTH];
	int colorBackIsAlpha[NUM_HUMAN_WIDTH];
};

enum Direction {
	NORTH = 0,// 配列の添え字にも使用しているので０基点
	EAST,
	SOUTH,
	WEST,
};

struct State {
	int			isDead;		// 死んでるなら１
	int			isAttack;	// 攻撃しているなら１
	Direction	dir;		// 向き
};

struct Human {
	char		name[NUM_HUMAN_NAME_MAX];	// キャラクターの名前
	int			health;						// 体力
	int			partsOrigin;				// parts[] の基点
	int			uid;						// ユニークＩＤ（使い道無し）
	int			gid;						// グループＩＤ
	State		state;						// キャラクターの状態
	Position	pos;						// キャラクターのデータ上の座標
	Position	printPos;					// キャラクターの描画上の座標
	Parts		parts[NUM_HUMAN_PARTS_SIZE];// キャラクターのパーツ
	Position	breadcrumbs[NUM_HUMAN_MAX_BREADCRUMBS];// ブレッドクラム経路
	// コールバック用
	struct {
		int (*callback)(Human *human, int dammage);
		int dammage;
	} applyDammage;
	struct {
		int (*callback)(Human *human, Direction dir);
		Direction dir;
	} knockBack;

	int (*Init)(Human* human);
	int (*Action)(Human* human);
	int (*Update)(Human* human);
	int (*Print)(Human* human);
};

/*-----------------------------------------------------------------------------
  Human: Prototype Extern Functions
  ---------------------------------------------------------------------------*/

extern int		HumanInit			(Human *human, const char *name, int gid, int y, int x, PartsRecord *parrec, int parrecSize);
extern int 		HumanUpdate			(Human *human);
extern int 		HumanPrint			(Human *human, int originY, int originX);
extern int 		HumanMove			(Human *human, int y, int x);
extern int  	HumanCanMove		(Human *human, int y, int x, int dir);
extern int  	HumanCanRotate		(Human *human, int dir);
extern int		HumanCollideTo		(Human *human, Human **collided, int y, int x, int dir);

extern Human *	HumanRaycast		(Human *human, int *dist, int height, int width);

extern int		HumanApplyDammage	(Human *human, int dmg);
extern int		HumanKnockBack		(Human *human, Direction dirFrom);
extern int		HumanCanPrintParts	(Human *human, Position *printPos, int partsIndex);

extern void 	HumanInitBreadCrumb (Human *human);
extern void		HumanDropBreadCrumb (Human *human);
extern int		HumanFindBreadCrumb (Human *human, Human *target, Position *foundpos);
extern int		HumanMoveToPos		(Human *human, Position topos, bool doRotate);

#endif/*__HUMAN_H_INCLUDED__*/

