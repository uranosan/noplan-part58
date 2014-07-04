#ifndef __PLAYER_H_INCLUDED__
#define __PLAYER_H_INCLUDED__

#include <stdio.h>
#include <stdbool.h>
#include "Human.h"
#include "Zombie.h"
#include "KeyInput.h"

/*-----------------------------------------------------------------------------
  Player: Global Numbers
  ---------------------------------------------------------------------------*/
enum TAG_NUM_PLAYER {
	NUM_PLAYER_HEALTH = 200,
};

/*-----------------------------------------------------------------------------
  Player: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int PlayerInit  (Human* player);
extern int PlayerUpdate(Human* player);
extern int PlayerPrint (Human* player);
extern int PlayerAction(Human* player);

#endif/*__PLAYER_H_INCLUDED__*/

