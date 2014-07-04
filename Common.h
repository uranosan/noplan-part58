#ifndef __COMMON_H_INCLUDED__
#define __COMMON_H_INCLUDED__

#include <stdlib.h>
#include <math.h>

/*-----------------------------------------------------------------------------
  Common: Global Macros
  ---------------------------------------------------------------------------*/
#define NELEMS(array) (sizeof(array)/sizeof(array[0]))

/*-----------------------------------------------------------------------------
  Common: Global Typedef
  ---------------------------------------------------------------------------*/
typedef struct Position Position;
typedef struct Colorset Colorset;

struct Position {
	int y, x;
};

struct Colorset {
	int fore;
	int back;
	int foreIsAlpha;// 透過させるなら１．でなければ０
	int backIsAlpha;// 透過させるなら１．でなければ０
};

/*-----------------------------------------------------------------------------
  Common: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int		CommonGetRandom(int min, int max);
extern double	CommonGetDistance(int ay, int ax, int by, int bx);
extern void		CommonBuildPathTo(Position *path, int pathSize, Position me, Position to);

#endif/*__COMMON_H_INCLUDED__*/

