#ifndef __MAP_H_INCLUDED__
#define __MAP_H_INCLUDED__

#include <stdio.h>
#include <stdbool.h>
#include "Common.h"
#include "Console.h"
#include "Human.h"
#include "Maze.h"

/*-----------------------------------------------------------------------------
  Map: Global Numbers
  ---------------------------------------------------------------------------*/
enum TAG_NUM_MAP {
	NUM_MAP_HEIGHT		= 64,
	NUM_MAP_WIDTH		= 64,

	/* COLLISION NUMBERS. */
	NUM_MAP_COLLISION_FIELD_VALUE_BEGIN = 10,
	NUM_MAP_FIELD_ROOF,
	NUM_MAP_COLLISION_FIELD_VALUE_END,

	/* NO COLLISION NUMBERS. */
	NUM_MAP_FIELD_EMPTY,
	
	NUM_MAP_FIELD_WALL,
	NUM_MAP_FIELD_FLOOR,

	NUM_MAP_KAIDAN_LIGHT,
	NUM_MAP_KAIDAN_DARK,
};

/*-----------------------------------------------------------------------------
  Map: Global Typedef
  ---------------------------------------------------------------------------*/
typedef struct Mapchip	Mapchip;
typedef struct Map		Map;

struct Mapchip {
	int			value;
	Colorset	color;
};

struct Map {
	Position	printStartPos;
	Position	printEndPos;
	Mapchip		field[NUM_MAP_HEIGHT][NUM_MAP_WIDTH];
};

/*---------------------------------------------------------------------------
  Map: Inline Functions.
  -------------------------------------------------------------------------*/
static inline bool MapIsCollisionFieldValue(int value)
{
	return (value > NUM_MAP_COLLISION_FIELD_VALUE_BEGIN &&
			value < NUM_MAP_COLLISION_FIELD_VALUE_END);
}

/*-----------------------------------------------------------------------------
  Map: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern void		MapInit				(void);
extern void		MapPrint			(void);
extern int		MapTraceFieldValue	(Position me, Position to);
extern int		MapGetFieldValue	(int y, int x);
extern Colorset MapGetFieldColor	(int y, int x);
extern Position MapGetPrintStartPos	(void);
extern Position MapGetPrintEndPos	(void);
extern void		MapSetPrintStartPos	(int y, int x);
extern void		MapSetPrintEndPos	(int y, int x);

extern Position MapGetStartSpawnPos(void);
extern Position MapGetGoalSpawnPos(void);

extern void MapCleanup(void);//here.
extern struct WayPointData* MapGetWayPointData(void);//here


#endif/*__MAP_H_INCLUDED__*/

