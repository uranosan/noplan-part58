#ifndef __MAZE_H_INCLUDED__
#define __MAZE_H_INCLUDED__

#include <stdbool.h>
#include "Common.h"
#include "Map.h"


typedef struct WayPoint {
	Position pos;
	struct WayPoint *links[4];
	int linksum;
} WayPoint;

struct WayPointRouteData {
	WayPoint **deadend;
	int deadendElems;
	WayPoint mostLongPair[2];
};

struct WayPointData {
	int orgy;
	int orgx;
	int height;
	int width;
	int interval;
	int elems;// *points の最大要素数。
	WayPoint *points;
	struct WayPointRouteData route;
};

/*-----------------------------------------------------------------------------
  Maze: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern unsigned int MazePathElems(int sy, int sx, int ey, int ex, int height, int width, int margin);
extern void 		MazeBuildPath(Position *path, int pathElems, int sy, int sx, int ey, int ex, int height, int width, int margin);

/* ウェイポイント */
extern int  MazeInitWayPointData( struct WayPointData *way, int orgy, int orgx, int height, int width, int interval);
extern void MazeAllocWayPoints( struct WayPointData *way );
extern void MazeFreeWayPoints( struct WayPointData *way );
extern void MazeAllocDeadend( struct WayPointData *way );
extern void MazeFreeDeadend( struct WayPointData *way );
extern void MazeBuildWayPoints( struct WayPointData *way);
extern void MazeSaveDeadend( struct WayPointData *way );
extern int  MazeFindMostLongRouteInDeadend( struct WayPointData *way, const int maxLimit );
extern void MazePrintWayPoint( WayPoint *point );

#endif/*__MAZE_H_INCLUDED__*/

