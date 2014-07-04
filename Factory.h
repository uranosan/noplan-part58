#ifndef __FACTORY_H_INCLUDED__
#define __FACTORY_H_INCLUDED__

#include "Human.h"
#include "Player.h"
#include "Zombie.h"
#include "Hiroin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Human* FactoryMakeHuman(const char* makename);

#endif/*__FACTORY_H_INCLUDED__*/

