#ifndef __HIROIN_H_INCLUDED__
#define __HIROIN_H_INCLUDED__

#include "Common.h"
#include "Human.h"
#include "Npc.h"
#include <stdbool.h>

/*-----------------------------------------------------------------------------
  Hiroin: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int HiroinInit  (Human* hiroin);
extern int HiroinUpdate(Human* hiroin);
extern int HiroinPrint (Human* hiroin);
extern int HiroinAction(Human* hiroin);

#endif/*__HIROIN_H_INCLUDED__*/

