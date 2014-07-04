#ifndef __NPC_H_INCLUDED__
#define __NPC_H_INCLUDED__

#include "Common.h"
#include "Human.h"
#include "Player.h"

typedef struct PartsRecord PartsRecord;

extern int NpcInit		(Human *human, const char *name, int gid, int y, int x, PartsRecord *parrec, int parrecSize);
extern int NpcUpdate	(Human *human);
extern int NpcPrint		(Human *human);
extern int NpcAction	(Human *human, int (*const actions[])(Human *human));

extern int	NpcActionRandomMove	(Human *human);
extern int	NpcActionAttack		(Human *human, int dammage, int doAttackMaxDist);
extern int	NpcActionChase		(Human *human, int doChaseMinDist, int doChaseMaxDist);

#endif/*__NPC_H_INCLUDED__*/

