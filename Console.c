#include "Console.h"

/*-----------------------------------------------------------------------------
  Console: Static Numbers
  ---------------------------------------------------------------------------*/
enum {
	NUM_MAX_STR_STACK	= 100,	/* ConsolePushStrStack で使うスタックの最大要素数 */
	NUM_MAX_STR			= 64,	/* 文字列最大要素数 */
};

/*-----------------------------------------------------------------------------
  Console: Static Variables
  ---------------------------------------------------------------------------*/
static HANDLE   m_hStdOut;
static int      m_hScreenIndex;
static HANDLE   m_hScreen[NUM_SCREEN_MAX];
static HANDLE   m_hWork;
static HANDLE   m_hOut;

static char		m_strStack[NUM_MAX_STR_STACK][NUM_MAX_STR];
static int		m_strStackTop = 0;

/*-----------------------------------------------------------------------------
  Console: Prototype Static Function
  ---------------------------------------------------------------------------*/
static void SetWindowSize(HANDLE hOut, int height, int width);

/*-----------------------------------------------------------------------------
  Console: Extern Functions
  ---------------------------------------------------------------------------*/
extern void ConsoleFillRect( int top, int left, int bottom, int right, int fgColor, int bgColor, const char* str)
{
	ConsoleSetColor( fgColor, bgColor );
	for (int i=top; i<bottom; ++i)
	{
		ConsoleSetCursor( i, 0 );
		for (int j=left; j<right; ++j)
		{
			ConsolePrint(str);
		}
	}
}


extern void ConsoleInit(void)
{
	CONSOLE_CURSOR_INFO cci;
	int i;
	
	/* コンソール標準出力画面のハンドルを取得。プログラム終了時にアクティブ化 */	
	m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if (m_hStdOut == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "error: Console: InitConsole: hStdOut: %ld\n", GetLastError());
	}
	
	/* コンソールスクリーンの作成と初期化 */
	for (i=0; i<NUM_SCREEN_MAX; i++)
	{
		m_hScreen[i] = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);
		if (m_hScreen[i] == INVALID_HANDLE_VALUE)
		{
			fprintf(stderr, "error: Console: InitConsole: %ld\n", GetLastError());
		}
		
		/* カーソル情報の初期化 */
		GetConsoleCursorInfo(m_hScreen[i], &cci);
		cci.bVisible = FALSE;// カーソルの非表示
		SetConsoleCursorInfo(m_hScreen[i], &cci);
		
		/* ウィンドウサイズの初期化 */
		SetWindowSize(m_hScreen[i], NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH);
		
		/* コンソールモードの初期化 */
		SetConsoleMode(m_hScreen[i], ENABLE_PROCESSED_OUTPUT);
	}
	m_hScreenIndex	= 0;
	m_hOut			= m_hScreen[1];
    m_hWork			= m_hScreen[0];
	SetConsoleActiveScreenBuffer(m_hOut);// 出力スクリーンをアクティブ化
}

extern void ConsoleClose(void)
{
	int i;
	
	for (i=0; i<NUM_SCREEN_MAX; i++)
	{
		CloseHandle(m_hScreen[i]);
		m_hScreen[i] = NULL;
	}
	m_hOut = m_hWork = NULL;
	
	SetConsoleActiveScreenBuffer(m_hStdOut);
	CloseHandle(m_hStdOut);
	m_hStdOut = NULL;
}

extern void ConsoleFlip(void)
{
	m_hOut = m_hWork;// 仕事を終えたハンドルを、表示用のハンドルに
	m_hScreenIndex = (m_hScreenIndex + 1) % NUM_SCREEN_MAX;// スクリーンインデックスを進める
	m_hWork = m_hScreen[m_hScreenIndex];// 次の仕事用バッファ
	
	SetConsoleActiveScreenBuffer(m_hOut);// 表示用のハンドルをアクティブ化
}

extern void ConsoleSwapScreen(void)
{
	HANDLE temp = m_hOut;
	m_hOut	= m_hWork;
	m_hWork = temp;
}

extern void ConsoleSetColor(int fgColor, int bgColor)
{
	SetConsoleTextAttribute(m_hWork, (fgColor + (bgColor * 16)) );
}

extern void ConsoleSetCursor(int y, int x)
{
	COORD coPos = {(SHORT)x, (SHORT)y};
	SetConsoleCursorPosition(m_hWork, coPos);
}

extern int ConsolePrint(const char * str)
{
	unsigned long cell = 0;
	WriteConsole(m_hWork, str, strlen(str), &cell, NULL);
	return (int)cell;
}

/* 書式付文字列をコンソール画面に出力。
 * ラッパ関数：ConsoleSetColor, ConsoleSetCursor, ConsolePrint
 *
 * @ret		書き込んだ文字列の長さ。
 * @param	y	書き込む画面位置Ｙ
 * @param	x	書き込む画面位置Ｘ
 * @param	fg	文字列の前景色
 * @param	bg	文字列の後景色
 * @param	fmt	書式
 */
extern int ConsolePrintFormat(int y, int x, int fg, int bg, const char *fmt, ...)
{
	char str[NUM_MAX_STR] = {};
	va_list args;
	
	va_start(args, fmt);
	vsnprintf(str, sizeof(str), fmt, args);
	va_end(args);
	
	ConsoleSetCursor(y, x);
	ConsoleSetColor(fg, bg);
	return ConsolePrint(str);
}


extern int ConsolePushStrStack(const char *format, ...)
{
	if (m_strStackTop >= NUM_MAX_STR_STACK) {
		return -1;
	} else {
		va_list args;
		va_start(args, format);
		
		int len = vsnprintf(m_strStack[m_strStackTop], sizeof(m_strStack[m_strStackTop]), format, args);
		m_strStackTop++;
		
		va_end(args);
		return len;
	}
}

extern const char * ConsolePopStrStack(void)
{
	if (m_strStackTop <= 0) {
		return NULL;
	} else {
		m_strStackTop--;
		return m_strStack[m_strStackTop];
	}
}

extern void ConsolePrintStrStack(void)
{
	const char *str;
	int i = 0;
	
	while ((str = ConsolePopStrStack()) != NULL)
	{
		ConsoleSetColor(WHITE, BLACK);
		ConsoleSetCursor(i++, 0);
		ConsolePrint(str);
	}
}

/*-----------------------------------------------------------------------------
  Console: Static Functions
  ---------------------------------------------------------------------------*/
static void SetWindowSize(HANDLE hOut, int height, int width)
{
	COORD		coPos = {(SHORT)width, (SHORT)height};
	SMALL_RECT  sr	  = {0, 0, (SHORT)(width-1), (SHORT)(height-1)};
	
	if (SetConsoleWindowInfo(hOut, TRUE, &sr) == 0) {
		fprintf(stderr, "error: SetConsoleWindowInfo: %ld\n", GetLastError());
	}
	if (SetConsoleScreenBufferSize(hOut, coPos) == 0) {
		fprintf(stderr, "error: SetConsoleScreenBufferSize: %ld\n", GetLastError());
	}
}

