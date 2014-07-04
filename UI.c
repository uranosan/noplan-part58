#include "UI.h"

/*-----------------------------------------------------------------------------
  UI: Menu: Static Numbers
  ---------------------------------------------------------------------------*/
enum TAG_NUM_MENU {
	NUM_MENU_ITEM_MAX = 3,
	
	NUM_MENU_WINDOW_Y		= NUM_WINDOW_HEIGHT / 3,
	NUM_MENU_WINDOW_X		= NUM_WINDOW_WIDTH / 3,
	NUM_MENU_WINDOW_HEIGHT	= NUM_WINDOW_HEIGHT / 3,
	NUM_MENU_WINDOW_WIDTH	= NUM_WINDOW_WIDTH / 3,
};

/*-----------------------------------------------------------------------------
  UI: Menu: Static Variables
  ---------------------------------------------------------------------------*/
static const char* m_menuitem[NUM_MENU_ITEM_MAX] = {
	"**  ゆっくり    **",
	"**  してってね  **",
	"**  Game Exit   **",
};
static int m_selectMenuNo;

/*-----------------------------------------------------------------------------
  UI: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static void MenuPrintBack(void);
static void MenuPrintItem(void);

/*-----------------------------------------------------------------------------
  UI: Static Functions
  ---------------------------------------------------------------------------*/
static void MenuPrintBack(void)
{
	int i, j;
	
	ConsoleSetColor(L_WHITE, BLACK);
	for (i = 0; i < NUM_MENU_WINDOW_HEIGHT; i++)
	{
		ConsoleSetCursor(NUM_MENU_WINDOW_Y + i, NUM_MENU_WINDOW_X);
		for (j = 0; j < NUM_MENU_WINDOW_WIDTH; j++)
		{
			ConsolePrint(" ");
		}
	}
}

static void MenuPrintItem(void)
{
	int i;
	
	for (i = 0; i < NUM_MENU_ITEM_MAX; i++)
	{
		ConsoleSetCursor( NUM_MENU_WINDOW_Y + i*2 + 2, NUM_MENU_WINDOW_X + 4);
		/* 選択中のメニューをハイライトして描画 */
		if (m_selectMenuNo == i)
			ConsoleSetColor(BLACK, L_WHITE);
		else
			ConsoleSetColor(L_WHITE, BLACK);
		
		ConsolePrint(m_menuitem[i]);
	}
}
/*-----------------------------------------------------------------------------
  UI: Extern Functions
  ---------------------------------------------------------------------------*/
extern void UIUpdate(void)
{
}

extern int UIMenuLoop(void)
{
	int key = 0,
		ret = 0;
	
	/* ダブルバッファ処理の停止 */
	ConsoleSwapScreen();
	/* メニュー画面の初期化 */
	MenuPrintBack();
	MenuPrintItem();
	
	/* メニューループ */
	while (1)
	{
		/* キー入力待ち */
		key = KeyWait();
		if (key == NUM_KEY_MENU)
			break;
		
		/* 選択メニューの処理 */
		if (key == NUM_KEY_ATTACK && m_selectMenuNo == 2)
		{
			ret = NUM_KEY_EXIT;/* 戻り値を格納してループから抜ける */
			break;
		}
		
		/* キー入力処理 */
		if (key == NUM_KEY_UP)
		{
			m_selectMenuNo = (NUM_MENU_ITEM_MAX + (m_selectMenuNo - 1)) % NUM_MENU_ITEM_MAX;
		}
		else if (key == NUM_KEY_DOWN)
		{
			m_selectMenuNo = (m_selectMenuNo + 1) % NUM_MENU_ITEM_MAX;
		}
		/* 描画 */
		MenuPrintItem();/* ダブルバッファを停止中なので、アイテムのみ描画 */
	}
	
	ConsoleSwapScreen();/* ダブルバッファ処理の再開 */
	return ret;/* ループ終了 */
}

extern void UIPrint(void)
{
	const Manager* manager = ManagerGetInstance();	
	double	barLen	= 20;/* 体力バーの長さ */
	double	redzone	= barLen / 4;/* 体力バーのレッドゾーン */
	int		barElems= (int)((double)manager->player->health / (double)NUM_PLAYER_HEALTH * barLen);// 体力から体力バーの個数を計算
	int		i;
	int		orgy = 1, orgx = 2;
	
	ConsolePrintFormat( orgy, orgx, L_WHITE, BLACK, "体力" );
	/* 体力バーの色を選択 */
	if (barElems <= redzone)/* 瀕死 */
	{
		ConsoleSetColor(BLACK, L_RED);
	}
	else/* 元気 */
	{
		ConsoleSetColor(BLACK, L_GREEN);
	}
	/* 体力バーの描画 */
	ConsoleSetCursor( orgy, orgx + 4);
	for (i = 0; i < barLen; i++)
	{
		if (i >= barElems) {
			ConsoleSetColor(BLACK, BLACK);
		}
		ConsolePrint(" ");
	}
}

