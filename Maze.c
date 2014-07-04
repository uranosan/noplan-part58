#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "Maze.h"

static const bool debug = true;

/*---------------------------------------------------------------------------
  Maze: Prototype Static Functions
  -------------------------------------------------------------------------*/
static void InitWayPoint( WayPoint *point, int y, int x);
static void AddWayPoint( WayPoint *point, WayPoint *addpoint);
static int  Lookup( WayPoint *point, WayPoint *goal, int limit, int depth);
static int  LookupStartToGoal(WayPoint *stat, WayPoint *goal, const int maxLimit);

/*-----------------------------------------------------------------------------
  Maze: Extern Functions
  ---------------------------------------------------------------------------*/

/* @ret		迷路パスの要素数
 * @param	sy, sx			1~
 * @param	ey, ex			0~
 * @param	height, width	1~
 * @param	margin			1~
 */
extern unsigned int MazePathElems(int sy, int sx, int ey, int ex, int height, int width, int margin)
{
	if (margin <= 0)
		margin = 1;
		
	int a = (height - sy - ey + 1) / margin;
	int b = (width - sx - ex + 1) / margin;
	return a * b * margin;
}

/* MazeBuildPath: 棒倒し法による迷路の内壁パスを作成。
 * @ret		迷路パスの要素数
 * @param	sy, sx			1~
 * @param	ey, ex			0~
 * @param	height, width	1~
 * @param	margin			1~
 */
extern void MazeBuildPath(Position *path, int pathElems, int sy, int sx, int ey, int ex, int height, int width, int margin)
{
	int i, j, k, orgY, orgX, pc;
	int dir, tmp, flag;
	int w = (width - sx - ex + 1);
	
	tmp = 0;
	pc  = 0;
	for (i = sy - 1 + margin - 1; i < height - ey; i += margin)
	{
		for (j = sx - 1 + margin - 1; j < width - ex; j += margin)
		{
			if (pc >= pathElems)
				return;
			orgY = path[pc].y = i;
			orgX = path[pc].x = j;
			pc++;
			
			// 鉛筆を倒す方向の決定
			flag = 0;
			do {
				if (pc < w) {
					dir = CommonGetRandom(1, 4);
				} else {
					dir = CommonGetRandom(2, 4);
				}
				if (abs(dir - tmp) == 2) {
					flag = 1;
				} else {
					flag = 0;
				}
			} while (flag);
			
			tmp = dir;
			
			
			// 鉛筆を倒す
			for (k = 1; k < margin; k++)
			{
				if (dir == 1) {// 上
					path[pc].y = orgY - k;
					path[pc].x = orgX;
					if (path[pc].y < 0 || path[pc].y >= height)
						path[pc].y = orgY;
				}
				else if (dir == 2) {// 右
					path[pc].y = orgY;
					path[pc].x = orgX + k;
					if (path[pc].x < 0 || path[pc].x >= width)
						path[pc].x = orgX;
				}
				else if (dir == 3) {// 下
					path[pc].y = orgY + k;
					path[pc].x = orgX;
					if (path[pc].y < 0 || path[pc].y >= height)
						path[pc].y = orgY;
				}
				else if (dir == 4) {// 左
					path[pc].y = orgY;
					path[pc].x = orgX - k;
					if (path[pc].x < 0 || path[pc].x >= width)
						path[pc].x = orgX;
				}
				pc++;
			}
		}
	}
}

/*---------------------------------------------------------------------------
  Maze WayPoint: ToDo
  -------------------------------------------------------------------------*/

/* WayPointData構造体を初期化する。
 * @return			作成するウェイポイントの総数。
 *
 * @param *way		WayPointData構造体へのポインタ。
 * @param orgy		ウェイポイントの作成を始める基点座標Y。
 * @param orgx		ウェイポイントの作成を始める基点座標X。
 * @param height	ウェイポイントを作成する範囲：高さ。（0 ~ height) | (orgy ~ height-orgy).
 * @param width		ウェイポイントを作成する範囲：幅。（0 ~ width) | (orgx ~ width-orgx).
 * @param interval	ウェイポイントを置いていく間隔。
*/
extern int MazeInitWayPointData( struct WayPointData *way, int orgy, int orgx, int height, int width, int interval)
{
	way->orgy = orgy;
	way->orgx = orgx;
	way->width = width;
	way->height = height;
	way->interval = interval;
	way->points = 0;
	way->elems = 0;

	struct WayPointRouteData emptyRouteData = {};
	way->route = emptyRouteData;

	// 要素数を計算
	int cnty = 0, cntx = 0;
	for (int i= way->orgy; i< way->height; i += way->interval) cnty++;
	for (int i= way->orgx; i< way->width ; i += way->interval) cntx++;
	way->elems = cnty * cntx;
	return way->elems;
}

/* ウェイポイントを初期化。
 * @param *point	初期化するウェイポイント。
 * @param y			ウェイポイントの座標Ｙ。
 * @param x			ウェイポイントの座標Ｘ。
*/
static void InitWayPoint( WayPoint *point, int y, int x)
{
	point->pos.y = y;
	point->pos.x = x;
	point->linksum = 0;
	for (int i= 0; i< NELEMS(point->links); ++i)
	{
		point->links[i] = 0;
	}
}

/* WayPoint 配列のメモリを確保。
 * @param *way WayPointData構造体のポインタ。
*/
extern void MazeAllocWayPoints( struct WayPointData *way )
{
	way->points = (WayPoint *) malloc(sizeof(WayPoint) * way->elems);
	if (! way->points) fprintf(stderr, "error: AllocWayPoints: malloc failde."), exit(2);
}

extern void MazeFreeWayPoints( struct WayPointData *way )
{
	if (way->points)
	{
		free(way->points), way->points = 0;
	}
}

extern void MazeAllocDeadend( struct WayPointData *way )
{
	way->route.deadend = (WayPoint **) malloc(
							sizeof(WayPoint *) * way->route.deadendElems );
	if (! way->route.deadend) fprintf(stderr,"error: AllocDeadend: malloc failed."), exit(2);
}

extern void MazeFreeDeadend( struct WayPointData *way )
{
	if (way->route.deadend)
	{
		free(way->route.deadend), way->route.deadend = 0;
	}
}

/* ウェイポイントを構築。
 * @param *way	初期化&メモリ確保済みのウェイポイントデータ構造体。
*/
extern void MazeBuildWayPoints( struct WayPointData *way)
{
	int orgy			= way->orgy;
	int orgx			= way->orgx;
	int height			= way->height;
	int width			= way->width;
	int interval		= way->interval;
	int pointsElems		= way->elems;
	WayPoint *points	= way->points;

	// 初期化
	int pc= 0;
	for (int y= orgy; y< height; y+= interval)
	{
		for (int x= orgx; x< width; x+= interval)
		{
			if (pc >= pointsElems)// エラー
				return;
			InitWayPoint( &points[pc], y, x);
			pc++;
		}
	}

	// ウェイポイントリストの作成
	WayPoint	*curpoint;
	Position	environs[4];
	bool		flags[4];

	way->route.deadendElems = 0;// 袋小路の総数を初期化

	for (int i= 0; i< pointsElems; ++i)
	{
		curpoint = &points[i];

		// 現在のポイントの、周辺座標を作成
		// NORTH
		environs[0].y = curpoint->pos.y - interval;
		environs[0].x = curpoint->pos.x;
		// SOUTH
		environs[1].y = curpoint->pos.y + interval;
		environs[1].x = curpoint->pos.x;
		// EAST
		environs[2].y = curpoint->pos.y;
		environs[2].x = curpoint->pos.x + interval;
		// WEST
		environs[3].y = curpoint->pos.y;
		environs[3].x = curpoint->pos.x - interval;

		// 現在のポイントから、作成した周辺座標とのあいだの
		// マップ上に、衝突物がないかを走査
		for (int dir= 0; dir< 4; ++dir)
		{
			Position fluct = {0, 0};// 変動の方向
			
			flags[dir] = true;

			// 変動の方向を決定
			switch (dir)
			{
				case 0:/* NORTH */ fluct.y = -1; fluct.x =  0; break;
				case 1:/* SOUTH */ fluct.y =  1; fluct.x =  0; break;
				case 2:/* EAST  */ fluct.y =  0; fluct.x =  1; break;
				case 3:/* WEST  */ fluct.y =  0; fluct.x = -1; break;
			}
			// マップ上の衝突物を走査
			for (int j= 0; j< interval; ++j)
			{
				int y = curpoint->pos.y + (j * fluct.y);
				int x = curpoint->pos.x + (j * fluct.x);

				if ((unsigned int)y >= NUM_MAP_HEIGHT ||
					(unsigned int)x >= NUM_MAP_WIDTH)
				{
					flags[dir] = false;// 有効範囲外
					break;
				}
				int value = MapGetFieldValue(y, x);
				if ( MapIsCollisionFieldValue( value ))
				{
					flags[dir] = false;// 衝突物有り。フラグをバッキバキ。
					break;
				}
			}
		}

		// 現在のポイントと、有効な周辺座標とをリンクする
		for (int j= 0; j< pointsElems; ++j)
		{
			if (curpoint == &points[j])
				continue;// 自身ならスルー

			for (int dir= 0; dir< 4; ++dir)
			{
				if (flags[dir])// その方向の周辺座標が有効で
				{
					// ポイントリスト内のポイントの座標と一致するなら
					if ( points[j].pos.y == environs[dir].y &&
						 points[j].pos.x == environs[dir].x )
					{
						// リンクする
						AddWayPoint( curpoint, &points[j] );
						break;
					}
				}
			}
		}
		if (curpoint->linksum == 1)
		{
			way->route.deadendElems++;
		}
	}
}

/* ウェイポイントにリンクを加える。すでにリンク済みであるポイントはリンクしない。リンクが満杯のときもリンクしない。
 * @param *point	ウェイポイント。
 * @param *addpoint 加えるポイント。
*/
static void AddWayPoint( WayPoint *point, WayPoint *addpoint)
{
	for (int i= 0; i< NELEMS(point->links); ++i)
	{
		if (point->links[i] == addpoint)
		{
			return;
		}
		if (point->links[i] == 0)
		{
			point->links[i] = addpoint;
			point->linksum++;
			return;
		}
	}
}

/* ウェイポイントを描画 */
extern void MazePrintWayPoint( WayPoint *point )
{
	printf("point(%2d,%2d)\n", point->pos.y, point->pos.x);

	for (int i= 0; i< NELEMS(point->links); ++i)
	{
		if (point->links[i])
		{
			printf("            -> link point(%2d,%2d)\n",
				point->links[i]->pos.y, point->links[i]->pos.x);
		}
	}
	puts("--------------------------------");
}

/* ウェイポイント中の袋小路（dead end) のアドレスをポインタ配列に格納する。
 * *deadend[] のメモリが確保されていない場合、警告を出してリターン（あまり意味がない）。 　
*/
extern void MazeSaveDeadend( struct WayPointData *way )
{
	if (! way->route.deadend)
	{
		fprintf(stderr, "warning: BuildDeadend: route.deadend is invalid.");
		return;
	}
	int j= 0;
	for (int i= 0; i< way->elems && j< way->route.deadendElems; ++i)
	{
		if (way->points[i].linksum == 1)
		{
			way->route.deadend[j++] = &way->points[i];
		}
	}
}

/* ウェイポイント中の袋小路において、最長経路を持つ袋小路のペアを求める。
 * LookupStartToGoal(), Lookup() を使用。
 *
 * @return	最長経路を発見できたら、そのペア間の距離（深さ）を返す。
 			失敗したら -1 を返す。
 * @param *way		ウェイポイントデータ構造体へのポインタ。
 * @param maxLimit	反復深化深さ優先探索における、深化の限界値。
*/
extern int MazeFindMostLongRouteInDeadend( struct WayPointData *way, const int maxLimit )
{
	WayPoint	*stat,
				*goal,
				**deadend = way->route.deadend
				;
	int			deadendElems = way->route.deadendElems,
				res = -1,
				long_index_i = 0,
				long_index_j = 0,
				cur_depth = -1
				;

	for (int i= 0; i< deadendElems - 1; ++i)
	{
		stat = deadend[i];

		for (int j= i + 1; j< deadendElems; ++j)
		{
			goal = deadend[j];

			res = LookupStartToGoal( stat, goal, maxLimit );// 探索を開始。
			if (res >= 0)// 探索結果を判定。
			{
				if (res > cur_depth)
				{
					cur_depth = res;
					long_index_i = i;
					long_index_j = j;
				}
				if (debug) puts("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			}
		}
	}
	if (debug)
	{
		printf("deadend elems: %d\n", deadendElems);
		printf("found most long route is: stat(%2d,%2d)-> goal(%2d,%2d)\n",
			deadend[long_index_i]->pos.y, deadend[long_index_i]->pos.x,
			deadend[long_index_j]->pos.y, deadend[long_index_j]->pos.x );
	}
	way->route.mostLongPair[0] = *deadend[long_index_i];
	way->route.mostLongPair[1] = *deadend[long_index_j];
	return cur_depth;
}

/* LookupStartToGoal() の相方。再帰処理を担当する。
 * param< *point > から param< *goal > に辿りつくまで再帰する。
 * 使用アルゴリズム：反復深化深さ優先探索。
 *
 * @return			goal を見つけた場合、その深さを返す。
 *					失敗した場合は -1。
 *
 * @param *point	探索を始めるウェイポイント。
 * @param *goal		目的となるウェイポイント。
 * @param limit		再帰の限界値。（default 1~n）
 * @param depth		再帰探索を始める深さの初期値。(default 0)
*/
static int Lookup( WayPoint *point, WayPoint *goal, int limit, int depth)
{
	if (point)
	{
		int res = 0;
		if (point == goal)
		{
			if (debug) printf("depth %2d: (%2d,%2d) <- found goal\n", depth, point->pos.y, point->pos.x );
			return depth;// found goal.
		}
		if (depth >= limit)
		{
			return -1;
		}
		for (int i= 0; i< NELEMS(point->links); ++i)
		{
			res = Lookup( point->links[i], goal, limit, depth + 1 );// 再帰
			if (res >= 0)
			{
				if (debug) printf("depth %2d: (%2d,%2d)\n", depth, point->pos.y, point->pos.x );
				return res;// found goal
			}
		}
	}
	return -1;
}

/* Lookup() の相方。探索を始める関数。
 * 1 ~ maxLimit の値で Lookuo() を呼び出す。
 * 使用アルゴリズム：反復深化深さ優先探索。
 *
 * @return			goal を見つけた場合、その深さを返す。
 *					失敗した場合は -1。
 * @param *stat		探索を始めるウェイポイント。
 * @param *goal		目的となるウェイポイント。
 * @param maxLimit	探索の限界深化値。
*/
static int LookupStartToGoal(WayPoint *stat, WayPoint *goal, const int maxLimit)
{
	int res = -1;
	for (int i= 1; i< maxLimit; ++i)
	{
		for (int j= 0; j< NELEMS(stat->links); ++j)
		{
			res = Lookup( stat->links[j], goal, i, 0 );
			if (res >= 0)
			{
				// found goal.
				if (debug) printf("found route: depth %2d:(%2d,%2d)->(%2d,%2d)\n", res, stat->pos.y, stat->pos.x, goal->pos.y, goal->pos.x );
				return res;
			}
		}
	}
	return res;
}

