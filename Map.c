#include "Map.h"

static const bool debug = false;

/*-----------------------------------------------------------------------------
  Map: Static Variables
  ---------------------------------------------------------------------------*/
static struct WayPointData m_waypointdata;

static Map		m_map;
static Mapchip	m_mapchip_roof  = { NUM_MAP_FIELD_ROOF,  {BLACK, BLACK},};
static Mapchip	m_mapchip_wall  = { NUM_MAP_FIELD_WALL,  {GRAY, GRAY},};
static Mapchip	m_mapchip_floor = { NUM_MAP_FIELD_FLOOR, {WHITE, WHITE},};

static Mapchip	m_mapchip_start = { 0, {L_YELLOW, L_YELLOW},};
static Mapchip	m_mapchip_goal  = { 0, {L_GREEN, L_GREEN},};

enum {
	NUM_KAIDAN_HEIGHT=5,
	NUM_KAIDAN_WIDTH=3,
};
#define E {NUM_MAP_FIELD_EMPTY,{0,0}}
#define L {NUM_MAP_KAIDAN_LIGHT,{L_WHITE,L_WHITE}}
#define D {NUM_MAP_KAIDAN_DARK,{GRAY,GRAY}}
static Mapchip m_kaidan[] = {
	E,E,L,
	E,L,L,
	L,L,D,
	L,D,D,
	D,D,D,
};
#undef E
#undef L
#undef D

/*-----------------------------------------------------------------------------
  Map: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static void ApplyMazePath(void);
static void ApplyMazeWayPoints(void);

/*-----------------------------------------------------------------------------
  Map: Static Functions
  ---------------------------------------------------------------------------*/
static void ApplyMazePath(void)
{
	Position	*path;
	int			pathElems;
	
	/* 迷路の作成 */
	pathElems = MazePathElems(2, 2, 9, 9, NUM_MAP_HEIGHT, NUM_MAP_WIDTH, 9);
	path = (Position *) malloc(sizeof(Position) * pathElems);
	if (! path)
	{
		fprintf(stderr, "Map: Maze Path is null.\n");
		exit(2);
	}
	MazeBuildPath(path, pathElems, 2, 2, 9, 9, NUM_MAP_HEIGHT, NUM_MAP_WIDTH, 9);
	
	for (int i= 0; i< pathElems; i++)
	{
		m_map.field[ path[i].y ][ path[i].x ] = m_mapchip_roof;//m_mapchip_wall;
		for (int j= 1; j< 4; ++j)
		{
			int value= m_map.field[ path[i].y + j ][ path[i].x ].value;
			if ( MapIsCollisionFieldValue( value ))
				break;
			else
				m_map.field[ path[i].y + j ][ path[i].x ] = m_mapchip_wall;
		}
	}
	free(path), path= NULL;
}

static void ApplyMazeWayPoints(void)
{
	int orgy		= 3,
		orgx		= 3,
		height		= NUM_MAP_HEIGHT - orgy,
		width		= NUM_MAP_WIDTH  - orgx,
		interval	= 9;

	int elems = MazeInitWayPointData( &m_waypointdata, orgy, orgx, height, width, interval );
	MazeAllocWayPoints( &m_waypointdata );// メモリ確保
	MazeBuildWayPoints( &m_waypointdata );// ウェイポイント作成
	// 袋小路
	MazeAllocDeadend( &m_waypointdata );
	MazeSaveDeadend( &m_waypointdata );

	// 袋小路中の最長経路
	const int m_findMaxLimit = 18;
	MazeFindMostLongRouteInDeadend( &m_waypointdata, m_findMaxLimit );

	if (debug)
	{
		WayPoint *curpoint;
		int y, x;

		printf("way points elems: %d\n", elems);
		for (int i= 0; i< m_waypointdata.elems; ++i)
		{
			curpoint = &m_waypointdata.points[i];

			y = curpoint->pos.y;
			x = curpoint->pos.x;
			m_map.field[y][x].value = 0;
			m_map.field[y][x].color.fore = L_RED;
			m_map.field[y][x].color.back = L_RED;

			MazePrintWayPoint( curpoint );
		}
		printf("dead end elems: %d\n", m_waypointdata.route.deadendElems );
		printf( "found most long route in deadend:"
				"(%2d,%2d) -> (%2d,%2d)\n"
				, m_waypointdata.route.mostLongPair[0].pos.y
				, m_waypointdata.route.mostLongPair[0].pos.x
				, m_waypointdata.route.mostLongPair[1].pos.y
				, m_waypointdata.route.mostLongPair[1].pos.x
				);
		Position start = m_waypointdata.route.mostLongPair[0].pos;
		Position goal  = m_waypointdata.route.mostLongPair[1].pos;
		m_map.field[ start.y ][ start.x ] = m_mapchip_start;
		m_map.field[ goal.y  ][ goal.x  ] = m_mapchip_goal;
	}
}
/*
static void SetRectChip(int top, int left, int bottom, int right, Mapchip *chip)
{
	// for (int i=goal.y; i<goal.y + 4 && i<NUM_MAP_HEIGHT; ++i)
	for (int i=top; i<bottom && i<NUM_MAP_HEIGHT; ++i)
	{
		// for (int j=goal.x; j<goal.x + 4 && j<NUM_MAP_WIDTH; ++j)
		for (int j=left; j<right && j<NUM_MAP_WIDTH; ++j)
		{
			// if (i == goal.y && j == goal.x || i == goal.y && j == goal.x + 4 ||
				// i == goal.y + 4 && j == goal.x || i == goal.y + 4 && j == goal.x + 4
			if ((i == top && j == left) || (i == top && j == right-1) ||
				(i == bottom-1 && j == left) || (i == bottom-1 && j == right-1))
				continue;
			m_map.field[ i ][ j ] = *chip;
		}
	}
}
*/
static void SetChipArray( int top, int left, int height, int width, int size, Mapchip array[])
{
	for (int i=0; i<height; ++i)
	{
		for (int j=0; j<width; ++j)
		{
			int index = i * width + j;
			if ( top + i >= NUM_MAP_HEIGHT || left + j >= NUM_MAP_WIDTH )
				continue;
			if ( index >= size )
				continue;
			if ( array[index].value == NUM_MAP_FIELD_EMPTY )
				continue;
			m_map.field[ top + i ][ left + j ] = array[ index ];
		}
	}
}

/*-----------------------------------------------------------------------------
  Map: Extern Functions
  ---------------------------------------------------------------------------*/
extern void MapInit(void)
{
	/* マップの初期化と周囲に壁を生成 */
	for (int i= 0; i< NUM_MAP_HEIGHT; i++)
	{
		for (int j= 0; j< NUM_MAP_WIDTH; j++)
		{
			m_map.field[i][j] = m_mapchip_floor;
			
			if (i == 0 || i == NUM_MAP_HEIGHT - 1 || j == 0 || j == NUM_MAP_WIDTH - 1)
			{
				m_map.field[i][j] = m_mapchip_roof;
			}
		}
	}
	for (int j= 1; j< NUM_MAP_WIDTH-1; ++j)
	{
		for (int k= 1; k< 4; ++k)
		{
			int value= m_map.field[k][j].value;
			if (MapIsCollisionFieldValue( value ))
				break;
			m_map.field[k][j] = m_mapchip_wall;
		}
	}
	/* 迷路を適用 */
	ApplyMazePath();
	/* ウェイポイントを作成 */
	ApplyMazeWayPoints();
	/* ゴール地点に階段のマップチップを設置。 */
	Position goal  = m_waypointdata.route.mostLongPair[1].pos;
	SetChipArray(
		goal.y, goal.x,
		NUM_KAIDAN_HEIGHT, NUM_KAIDAN_WIDTH,
		NELEMS(m_kaidan), m_kaidan
	);
}

extern int MapGetFieldValue(int y, int x)
{
	return m_map.field[y][x].value;
}

extern Colorset MapGetFieldColor(int y, int x)
{
	return m_map.field[y][x].color;
}

/**
 * マップを描画
 */
extern void MapPrint(void)
{
	int ci, i, j;
	int sy, ey, sx, ex;
	
	sy = m_map.printStartPos.y;
	ey = m_map.printEndPos.y;
	sx = m_map.printStartPos.x;
	ex = m_map.printEndPos.x;
	
	for (ci=0, i=sy; i<ey; ci++, i++)
	{
		ConsoleSetCursor(ci, 0);
		for (j=sx; j<ex; j++)
		{
			ConsoleSetColor(m_map.field[i][j].color.fore, m_map.field[i][j].color.back);
			switch (m_map.field[i][j].value)
			{
				case NUM_MAP_FIELD_ROOF:	ConsolePrint("■"); break;
				case NUM_MAP_FIELD_WALL:	ConsolePrint("■"); break;
				case NUM_MAP_FIELD_FLOOR:	ConsolePrint("　"); break;
				default: 					ConsolePrint("　"); break;
			}
		}
	}
}

extern Position MapGetPrintStartPos(void)
{
	return m_map.printStartPos;
}

extern Position MapGetPrintEndPos(void)
{
	return m_map.printEndPos;
}

extern void MapSetPrintStartPos(int y, int x)
{
	m_map.printStartPos.y = y;
	m_map.printStartPos.x = x;
}

extern void MapSetPrintEndPos(int y, int x)
{
	m_map.printEndPos.y = y;
	m_map.printEndPos.x = x;
}

/* 始点から終点までのパスを作成後、パスの座標からマップのフィールドの値を調べる
 * @ret			フィールドの値が０なら０を返す。０でなければ１を返す
 * @param me	走査の始点座標
 * @param to	走査の終点座標
 */
extern int MapTraceFieldValue(Position me, Position to)
{
	int i;
	Position path[ 20 ] = {};
	Position mepos, topos;
	
	// パス作成座標の調整
	mepos.y = me.y + NUM_HUMAN_HEIGHT / 2;
	mepos.x = me.x + NUM_HUMAN_WIDTH / 2;
	topos.y = to.y + NUM_HUMAN_HEIGHT / 2;
	topos.x = to.x + NUM_HUMAN_WIDTH / 2;
	
	CommonBuildPathTo( path, NELEMS(path), mepos, topos);// パスを作成
	
	for (i=0; i<NELEMS(path) && path[i].y != -1 && path[i].x != -1; i++)
	{
		int value = m_map.field[ path[i].y ][ path[i].x ].value;
		if (MapIsCollisionFieldValue( value )) {
			return 0;// 衝突物あり
		}
	}
	return 1;// 衝突物なし
}

extern Position MapGetStartSpawnPos(void)
{
	return m_waypointdata.route.mostLongPair[0].pos;
}

extern Position MapGetGoalSpawnPos(void)
{
	return m_waypointdata.route.mostLongPair[1].pos;
}

extern void MapCleanup(void)//here
{
	MazeFreeDeadend( &m_waypointdata );
	MazeFreeWayPoints( &m_waypointdata );
}

extern struct WayPointData* MapGetWayPointData(void)//here.
{
	return &m_waypointdata;
}

