#include "GameMain.h"
#include "Main.h"

int main()
{
	int res = 0;

	ConsoleInit();	
	GameInit();

	while (true)
	{
		res = GameMenu();
		if (res == NUM_GAME_EXIT)
		{
			break;
		}
		else if (res == NUM_GAME_START)
		{
			res = GameMain();
			if (res == NUM_GAME_EXIT)
			{
				break;
			}
			else if (res == NUM_GAME_ENDING)
			{
				GameEnding();
				GameTerminate();
				GameInit();
			}
			else if (res == NUM_GAME_RESTART)
			{
				GameTerminate();
				GameInit();
			}
		}
	}
	GameTerminate();
	ConsoleClose();
	return 0;
}

