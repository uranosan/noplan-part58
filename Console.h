#ifndef __CONSOLE_H_INCLUDED__
#define __CONSOLE_H_INCLUDED__

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>

/*-----------------------------------------------------------------------------
  Console: Global Numbers
  ---------------------------------------------------------------------------*/
enum TAG_NUM_CONSOLE {
    NUM_WINDOW_HEIGHT		= 30,	/* コンソール画面の高さ */
    NUM_WINDOW_WIDTH		= 80,	/* コンソール画面の横幅 */
    NUM_SCREEN_MAX			= 2,	/* 作成するコンソールスクリーンの上限 */
};

enum TAG_NUM_CONSOLE_COLOR {
	BLACK = 0,
	BLUE, GREEN, CYAN, RED, PINK, YELLOW, WHITE, GRAY,
	L_BLUE, L_GREEN, L_CYAN, L_RED, L_PINK, L_YELLOW, L_WHITE,
	
	DBLA = 0,
	DBLU, DGRE, DCYA, DRED, DPIN, DYEL, DWHI, DGRA,
	LBLU, LGRE, LCYA, LRED, LPIN, LYEL, LWHI,
};

/*-----------------------------------------------------------------------------
  Console: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern void			ConsoleInit	(void);
extern void			ConsoleClose(void);
extern void			ConsoleFlip	(void);

extern void			ConsoleSwapScreen(void);

extern void			ConsoleSetColor(int fgColor, int bgColor);
extern void			ConsoleSetCursor(int y, int x);
extern int 			ConsolePrint(const char * str);
extern int			ConsolePrintFormat(int y, int x, int fg, int bg, const char *fmt, ...);

extern int			ConsolePushStrStack	(const char *format, ...);
extern const char * ConsolePopStrStack	(void);
extern void			ConsolePrintStrStack(void);

extern void 		ConsoleFillRect( int top, int left, int bottom, int right, int fgColor, int bgColor, const char* str);

#endif/*__CONSOLE_H_INCLUDED__*/

