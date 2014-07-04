#ifndef __GAMEMAIN_H_INCLUDED__
#define __GAMEMAIN_H_INCLUDED__

#include "Manager.h"
#include "Factory.h"

#include "Common.h"
#include "KeyInput.h"
#include "Map.h"
#include "Human.h"
#include "Player.h"
#include "Hiroin.h"
#include "Zombie.h"
#include "Effect.h"
#include "UI.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

enum {
	NUM_GAME_START=1,
	NUM_GAME_RESTART,
	NUM_GAME_EXIT,
	NUM_GAME_ENDING,
};

/*-----------------------------------------------------------------------------
  GameMain: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int GameMain();
extern void GameInit(void);
extern void GameTerminate(void);

extern int GameMenu(void);
extern void GameEnding(void);


#endif/*__GAMEMAIN_H_INCLUDED__*/

