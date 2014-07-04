#include "Factory.h"

extern Human* FactoryMakeHuman(const char* makename)
{
	Human* newhuman = (Human*) malloc(sizeof(Human));
	if (!newhuman) {
		perror(makename);
		exit(1);
	}
	if (strcmp(makename, "Player") == 0) {
		PlayerInit(newhuman);
	}
	else if (strcmp(makename, "Zombie") == 0) {
		ZombieInit(newhuman);
	}
	else if (strcmp(makename, "Hiroin") == 0) {
		HiroinInit(newhuman);
	}
	else {
		ZombieInit(newhuman);
	}
	return newhuman;
}

