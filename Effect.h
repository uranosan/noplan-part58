#ifndef __EFFECT_H_INCLUDED__
#define __EFFECT_H_INCLUDED__

#include "Console.h"
#include "Common.h"
#include "List.h"
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------------------------------------
  Effect: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern void EffectPrint();
extern void EffectCleanup();
extern void EffectPopupMessage(const char *msg, int y, int x, int fgColor, int bgColor);

#endif/*__EFFECT_H_INCLUDED__*/

