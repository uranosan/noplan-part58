#include "GameMain.h"

/*---------------------------------------------------------------------------
  GameMain: Static Variables.
  -------------------------------------------------------------------------*/
static const bool debug = false;

static bool m_playerIsGoal = false;

enum {
	NUM_DEFAULT_HUMANS		= 5,
	NUM_DEFAULT_ZOMBIES 	= 3,
	NUM_ZOMBIE_SPAWN_STAGE	= 3,
	NUM_INIT_STAGE			= 0,
	NUM_MAX_STAGE			= 20,
};
static Manager* m_manager;

static int m_maxStage;
static int m_currentStage;

/*-----------------------------------------------------------------------------
  GameMain: Prototype Static Functions
  ---------------------------------------------------------------------------*/
static void GameUpdate(void);
static void GamePrint(void);
static void GameUpdatePrintPos(void);
static void GameReStart(void);

/*-----------------------------------------------------------------------------
  GameMain: Extern Functions
  ---------------------------------------------------------------------------*/

static int PrintPointerStrings(int top, int left, int fg, int bg, const char* title[], const int titleMax)
{
	int i=0;

	ConsoleSetColor(fg, bg);
	for (i=0; i<titleMax; ++i)
	{
		ConsoleSetCursor(top+i, left);
		ConsolePrint(title[i]);
	}
	return top+i;
}

static int PrintMenu(int top, int left, int fg, int bg, const char* menus[], const int menusMax, int cursor)
{
	int i=0;

	for (int i=0; i<menusMax; ++i)
	{
		if (cursor == i)
			ConsoleSetColor(bg, fg);
		else
			ConsoleSetColor(fg, bg);
		ConsoleSetCursor(top+i, left);
		ConsolePrint(menus[i]);
	}
	return top+i;
}

extern int GameMenu(void)
{
	const char* title[] = {
		"T H E                             ",
		"  *  *  **   ***  *     **  *  *  ",
		"  ** * *  *  *  * *    *  * ** *  ",
		"  * ** *  *  ***  *    **** * **  ",
		"  *  *  **   *    **** *  * *  *  ",
	};
	const char* menus[] = {
		"*** STAT ***",
		"*** EXIT ***",
	};
	const int titleMax = NELEMS(title);
	const int menusMax = NELEMS(menus);
	const int titleWidth = strlen(title[0]);
	const int menusWidth = strlen(menus[0]);
	const int titley = 10;
	const int titlex = NUM_WINDOW_WIDTH/2 - titleWidth/2;
	const int menusx = NUM_WINDOW_WIDTH/2 - menusWidth/2;
	const int menusPaddingTop = 4;
	int key;
	int cursor = 0;

	// メニュー画面初期化。
	ConsoleFillRect( 0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	int endy = PrintPointerStrings(titley, titlex, L_WHITE, BLACK, title, titleMax);
	PrintMenu(endy + menusPaddingTop, menusx, L_WHITE, BLACK, menus, menusMax, cursor);
	ConsoleFlip();

	while (true)
	{
		// 入力。
		key = KeyWait();
		// 更新。
		if (key == NUM_KEY_EXIT)
			return NUM_GAME_EXIT;
		else if (key == NUM_KEY_UP)
			cursor = (menusMax + (cursor - 1)) % menusMax;
		else if (key == NUM_KEY_DOWN)
			cursor = (cursor + 1) % menusMax;
		else if (key == NUM_KEY_ATTACK)
		{
			if (cursor == 0) return NUM_GAME_START;
			if (cursor == 1) return NUM_GAME_EXIT;
		}

		// 描画。
		ConsoleFillRect( 0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
		int endy = PrintPointerStrings(titley, titlex, L_WHITE, BLACK, title, titleMax);
		PrintMenu(endy + menusPaddingTop, menusx, L_WHITE, BLACK, menus, menusMax, cursor);
		ConsoleFlip();	
	}
	return NUM_GAME_EXIT;
}

extern void GameEnding(void)
{
	const char* youwin[] = {
		"*  *  **  *  *  *   * **** *  * ** **",
		"*  * *  * *  *  * * *  **  ** * ** **",
		" **  *  * *  *  * * *  **  * **      ",
		" **   **   **    * *  **** *  * ** **",
	};
	const int youwinMax = NELEMS(youwin);
	const int youwinWidth = strlen(youwin[0]);
	const int youwiny = 10;
	const int youwinx = NUM_WINDOW_WIDTH/2 - youwinWidth/2;

	// ダブルバッファの掃除。
	ConsoleFillRect(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	ConsoleFlip();
	// エンディング画面の初期化。	
	ConsoleFillRect(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	PrintPointerStrings(youwiny, youwinx, L_WHITE, BLACK, youwin, youwinMax);
	ConsoleFlip();

	while (true)
	{
		// 入力。
		int key = KeyWait();
		// 更新。
		// 二回攻撃ボタンを押したら終了。
		if (key == NUM_KEY_ATTACK)
		{
			if (KeyWait() == NUM_KEY_ATTACK)
				break;
		}
		// 描画。
		ConsoleFillRect
			(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
		PrintPointerStrings(youwiny, youwinx, L_WHITE, BLACK, youwin, youwinMax);
		ConsoleFlip();
	}
}

// ゲームエントリー
extern int GameMain(void)
{
	// プログラム開始時の処理
	GameUpdate();
	GamePrint();

	// ゲームループ
	while (1)
	{
		// キー入力があるまで待機
		int key = KeyWait();

		if (key == NUM_KEY_EXIT)
		{
			break;
		}
		else if (key > NUM_KEY_WRAP_START && key < NUM_KEY_WRAP_END)
		{
			// 全てのキャラクターを行動させる。
			m_manager->ActionAll();
			// ゲーム更新
			GameUpdate();
			// ゲーム描画
			GamePrint();
		}
		else if (key == NUM_KEY_MENU)
		{
			// メニュー無限ループ 
			if (UIMenuLoop() == NUM_KEY_EXIT)
				return NUM_GAME_RESTART;
			GamePrint();// メニューの表示をお掃除 
		}
		else if (key == NUM_KEY_STAGE_CHANGE)//debug
		{
			m_playerIsGoal = true;
		}
		else if (key == NUM_KEY_DEBUG_MAXSTAGE_BEFORE)//debug
		{
			m_currentStage = m_maxStage - 1;
			GameReStart();
		}
		else if (key == NUM_KEY_DEBUG_1)//debug
		{
			m_currentStage += NUM_ZOMBIE_SPAWN_STAGE;
			// 現在のステージカウントが最大数以上だったら、
			if ( m_currentStage >= m_maxStage )
			{
				return NUM_GAME_ENDING;
			}
			GameReStart();
		}

		if (m_playerIsGoal)
		{
			m_currentStage++;
			// 現在のステージカウントが最大数以上だったら、
			if ( m_currentStage >= m_maxStage )
			{
				return NUM_GAME_ENDING;
			}
			GameReStart();
		}
	}
	return 0;
}

extern void GameTerminate(void)
{
	m_manager->Cleanup();
	MapCleanup();
	EffectCleanup();
}

/*-----------------------------------------------------------------------------
  GameMain: Static Functions
  ---------------------------------------------------------------------------*/

static void GameReStart(void)
{
	const char* message = "now loading...";

	/*　「now loading...」を画面中央に表示する。　*/
	GamePrint();
	ConsolePrintFormat(
		NUM_WINDOW_HEIGHT / 2,
		NUM_WINDOW_WIDTH / 2 - strlen(message) / 2,
		L_WHITE,
		BLACK,
		message
	);
	ConsoleFlip();

	/* ゴールフラグの初期化 */
	m_playerIsGoal = false;
	/* マップ、キャラクターの初期化 */
	MapCleanup();
	MapInit();
	// 現在のステージが、３の倍数だったら、
	if ( m_currentStage > 0 && m_currentStage % NUM_ZOMBIE_SPAWN_STAGE == 0 )
	{
		// ゲームにゾンビをN体追加。
		if (debug) ConsolePushStrStack( "Add Zombie. %s %d",__FILE__,__LINE__);
		m_manager->AddHuman( m_manager->MakeHuman("Zombie") );
	}
	m_manager->InitAll();// 全てのキャラクターを初期化。
	m_manager->SpawnAll();// 全てのキャラクターを配置。
	/*　更新と描画　*/
	GameUpdate();
	GamePrint();
}

// ゲームを初期化
extern void GameInit(void)
{
	m_manager = (Manager*) ManagerGetInstance();
	srand((unsigned int)time(NULL));
	/* 状態の初期化。 */
	m_currentStage = NUM_INIT_STAGE;
	/* マップの初期化。 */
	MapInit();
	/* マップのウェイポイントからステージの最大数を計算。 */
	struct WayPointData* wpd = MapGetWayPointData();
	m_maxStage = (wpd->elems - NUM_DEFAULT_HUMANS) * NUM_ZOMBIE_SPAWN_STAGE;
	m_maxStage = (NUM_MAX_STAGE % m_maxStage);

	/* キャラクターを作成、マネージャーの配列に格納。 */
	m_manager->AddHuman( m_manager->MakeHuman("Player") );
	m_manager->AddHuman( m_manager->MakeHuman("Hiroin") );
	for (int i=0; i<NUM_DEFAULT_ZOMBIES; ++i)
		m_manager->AddHuman( m_manager->MakeHuman("Zombie") );
	/* 全てのキャラクターの初期座標を設定。 */
	m_manager->SpawnAll();
}

// ゲーム更新
static void GameUpdate(void)
{
	// キャラクター全てを更新。
	m_manager->UpdateAll();
	// プレイヤーが死んでいるならスタート地点に強制送還
	if ( m_manager->player->state.isDead )
	{
		PlayerInit( m_manager->player );
		PlayerUpdate( m_manager->player );
		EffectPrint();// エフェクトの描画（スタックを全てポップ）
	}
	// 描画位置を更新
	GameUpdatePrintPos();
	// UI
	UIUpdate();
	// ゴール判定。
	Position goalPos   = MapGetGoalSpawnPos();	
	Position playerPos = m_manager->player->pos;
	if (playerPos.y == goalPos.y && playerPos.x == goalPos.x)
	{
		Position hiroinPos = m_manager->hiroin->pos;
		if (abs(playerPos.y - hiroinPos.y) < 5 &&
			abs(playerPos.x - hiroinPos.x) < 5)
		{
			if (debug) ConsolePushStrStack("G O A L ! !");
			m_playerIsGoal = true;
		}
	}
}

// ゲーム描画
static void GamePrint(void)
{
	MapPrint();
	m_manager->PrintAll();

	EffectPrint();// エフェクトの描画（スタックを全てポップ）
	UIPrint();
	if (debug)
	{
		ConsolePushStrStack( "cur stage : %d/%d, humans : %d",
			m_currentStage,
			m_maxStage,
			m_manager->humansSize
		);
	}	
	ConsolePrintStrStack();//debug
	
	ConsoleFlip();// 描画済みの画面と表示中の画面を切り替え
}

// ゲーム内オブジェクトの描画位置を更新
static void GameUpdatePrintPos(void)
{
	const int	areaHeight = NUM_WINDOW_HEIGHT; 
	const int	areaWidth  = NUM_WINDOW_WIDTH / 2;
	int			sy, ey, sx, ex;	// マップの描画位置オフセット
	int			py, px;			// プレイヤの描画位置
	Position	playPos = {};
	playPos = m_manager->player->pos;

	const int say = NUM_HUMAN_HEIGHT / 2;
	const int sax = NUM_HUMAN_WIDTH / 2;

	// プレイヤーの位置からマップの描画オフセットと、プレイヤーの描画位置を獲得 
	// Y軸
	if (playPos.y < areaHeight / 2 - say)// プレイヤーがウィンドウ上端にいる場合
	{
		sy = 0;
		ey = areaHeight;
		py = playPos.y;
	}
	else if (playPos.y > (NUM_MAP_HEIGHT - areaHeight / 2) - say)// 下端
	{
		sy = NUM_MAP_HEIGHT - areaHeight;
		ey = NUM_MAP_HEIGHT;
		py = areaHeight / 2 + (areaHeight / 2 - (NUM_MAP_HEIGHT - playPos.y));
	}
	else// それ以外
	{
		sy = playPos.y - areaHeight / 2 + say;
		ey = playPos.y + areaHeight / 2 + say;
		py = areaHeight / 2 - say;
	}
	
	// X軸
	if (playPos.x < areaWidth / 2 - sax){// 左端
	
		sx = 0;
		ex = areaWidth;
		px = playPos.x;
	}
	else if (playPos.x > (NUM_MAP_WIDTH - areaWidth / 2) - sax)//右端
	{
		sx = NUM_MAP_WIDTH - areaWidth;
		ex = NUM_MAP_WIDTH;
		px = areaWidth / 2 + (areaWidth / 2 - (NUM_MAP_WIDTH - playPos.x));
	}
	else// それ以外
	{
		sx = playPos.x - areaWidth / 2 + sax;
		ex = playPos.x + areaWidth / 2 + sax;
		px = areaWidth / 2 - sax;
	}
	// 獲得した描画位置をセット
	m_manager->player->printPos.y = py;
	m_manager->player->printPos.x = px;
	MapSetPrintStartPos	(sy, sx);
	MapSetPrintEndPos	(ey, ex);
}

