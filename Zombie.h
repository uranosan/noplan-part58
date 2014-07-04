#ifndef __ZOMBIE_H_INCLUDED__
#define __ZOMBIE_H_INCLUDED__

#include "Common.h"
#include "Human.h"
#include "Npc.h"

/*-----------------------------------------------------------------------------
  Zombie: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int ZombieInit  (Human* zombie);
extern int ZombieUpdate(Human* zombie);
extern int ZombiePrint (Human* zombie);
extern int ZombieAction(Human* zombie);

#endif/*__ZOMBIE_H_INCLUDED__*/
