#ifndef __MANAGER_H_INCLUDED__
#define __MANAGER_H_INCLUDED__

typedef struct Human Human;// 前方参照。

#include "Common.h"
#include "Factory.h"
#include "Map.h"
#include "Maze.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	NUM_MANAGER_HUMANS_MAX = 100,
};
typedef struct Manager {
	Human* humans[NUM_MANAGER_HUMANS_MAX];
	int humansSize;
	int uniqueID;

	Human* player;
	Human* hiroin;

	Human*	(*const MakeHuman)(const char* makename);
	int		(*const AddHuman)(Human* addhuman);
	void	(*const Cleanup)(void);
	int		(*const InitAll)(void);
	int		(*const ActionAll)(void);
	int		(*const UpdateAll)(void);
	int		(*const PrintAll)(void);
	int		(*const SpawnAll)(void);
} Manager;

extern const Manager* ManagerGetInstance(void);

#endif/*__MANAGER_H_INCLUDED__*/

