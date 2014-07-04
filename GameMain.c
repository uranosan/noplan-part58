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

	// ���j���[��ʏ������B
	ConsoleFillRect( 0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	int endy = PrintPointerStrings(titley, titlex, L_WHITE, BLACK, title, titleMax);
	PrintMenu(endy + menusPaddingTop, menusx, L_WHITE, BLACK, menus, menusMax, cursor);
	ConsoleFlip();

	while (true)
	{
		// ���́B
		key = KeyWait();
		// �X�V�B
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

		// �`��B
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

	// �_�u���o�b�t�@�̑|���B
	ConsoleFillRect(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	ConsoleFlip();
	// �G���f�B���O��ʂ̏������B	
	ConsoleFillRect(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
	PrintPointerStrings(youwiny, youwinx, L_WHITE, BLACK, youwin, youwinMax);
	ConsoleFlip();

	while (true)
	{
		// ���́B
		int key = KeyWait();
		// �X�V�B
		// ���U���{�^������������I���B
		if (key == NUM_KEY_ATTACK)
		{
			if (KeyWait() == NUM_KEY_ATTACK)
				break;
		}
		// �`��B
		ConsoleFillRect
			(0, 0, NUM_WINDOW_HEIGHT, NUM_WINDOW_WIDTH, BLACK, BLACK, " ");
		PrintPointerStrings(youwiny, youwinx, L_WHITE, BLACK, youwin, youwinMax);
		ConsoleFlip();
	}
}

// �Q�[���G���g���[
extern int GameMain(void)
{
	// �v���O�����J�n���̏���
	GameUpdate();
	GamePrint();

	// �Q�[�����[�v
	while (1)
	{
		// �L�[���͂�����܂őҋ@
		int key = KeyWait();

		if (key == NUM_KEY_EXIT)
		{
			break;
		}
		else if (key > NUM_KEY_WRAP_START && key < NUM_KEY_WRAP_END)
		{
			// �S�ẴL�����N�^�[���s��������B
			m_manager->ActionAll();
			// �Q�[���X�V
			GameUpdate();
			// �Q�[���`��
			GamePrint();
		}
		else if (key == NUM_KEY_MENU)
		{
			// ���j���[�������[�v 
			if (UIMenuLoop() == NUM_KEY_EXIT)
				return NUM_GAME_RESTART;
			GamePrint();// ���j���[�̕\�������|�� 
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
			// ���݂̃X�e�[�W�J�E���g���ő吔�ȏゾ������A
			if ( m_currentStage >= m_maxStage )
			{
				return NUM_GAME_ENDING;
			}
			GameReStart();
		}

		if (m_playerIsGoal)
		{
			m_currentStage++;
			// ���݂̃X�e�[�W�J�E���g���ő吔�ȏゾ������A
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

	/*�@�unow loading...�v����ʒ����ɕ\������B�@*/
	GamePrint();
	ConsolePrintFormat(
		NUM_WINDOW_HEIGHT / 2,
		NUM_WINDOW_WIDTH / 2 - strlen(message) / 2,
		L_WHITE,
		BLACK,
		message
	);
	ConsoleFlip();

	/* �S�[���t���O�̏����� */
	m_playerIsGoal = false;
	/* �}�b�v�A�L�����N�^�[�̏����� */
	MapCleanup();
	MapInit();
	// ���݂̃X�e�[�W���A�R�̔{����������A
	if ( m_currentStage > 0 && m_currentStage % NUM_ZOMBIE_SPAWN_STAGE == 0 )
	{
		// �Q�[���Ƀ]���r��N�̒ǉ��B
		if (debug) ConsolePushStrStack( "Add Zombie. %s %d",__FILE__,__LINE__);
		m_manager->AddHuman( m_manager->MakeHuman("Zombie") );
	}
	m_manager->InitAll();// �S�ẴL�����N�^�[���������B
	m_manager->SpawnAll();// �S�ẴL�����N�^�[��z�u�B
	/*�@�X�V�ƕ`��@*/
	GameUpdate();
	GamePrint();
}

// �Q�[����������
extern void GameInit(void)
{
	m_manager = (Manager*) ManagerGetInstance();
	srand((unsigned int)time(NULL));
	/* ��Ԃ̏������B */
	m_currentStage = NUM_INIT_STAGE;
	/* �}�b�v�̏������B */
	MapInit();
	/* �}�b�v�̃E�F�C�|�C���g����X�e�[�W�̍ő吔���v�Z�B */
	struct WayPointData* wpd = MapGetWayPointData();
	m_maxStage = (wpd->elems - NUM_DEFAULT_HUMANS) * NUM_ZOMBIE_SPAWN_STAGE;
	m_maxStage = (NUM_MAX_STAGE % m_maxStage);

	/* �L�����N�^�[���쐬�A�}�l�[�W���[�̔z��Ɋi�[�B */
	m_manager->AddHuman( m_manager->MakeHuman("Player") );
	m_manager->AddHuman( m_manager->MakeHuman("Hiroin") );
	for (int i=0; i<NUM_DEFAULT_ZOMBIES; ++i)
		m_manager->AddHuman( m_manager->MakeHuman("Zombie") );
	/* �S�ẴL�����N�^�[�̏������W��ݒ�B */
	m_manager->SpawnAll();
}

// �Q�[���X�V
static void GameUpdate(void)
{
	// �L�����N�^�[�S�Ă��X�V�B
	m_manager->UpdateAll();
	// �v���C���[������ł���Ȃ�X�^�[�g�n�_�ɋ�������
	if ( m_manager->player->state.isDead )
	{
		PlayerInit( m_manager->player );
		PlayerUpdate( m_manager->player );
		EffectPrint();// �G�t�F�N�g�̕`��i�X�^�b�N��S�ă|�b�v�j
	}
	// �`��ʒu���X�V
	GameUpdatePrintPos();
	// UI
	UIUpdate();
	// �S�[������B
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

// �Q�[���`��
static void GamePrint(void)
{
	MapPrint();
	m_manager->PrintAll();

	EffectPrint();// �G�t�F�N�g�̕`��i�X�^�b�N��S�ă|�b�v�j
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
	
	ConsoleFlip();// �`��ς݂̉�ʂƕ\�����̉�ʂ�؂�ւ�
}

// �Q�[�����I�u�W�F�N�g�̕`��ʒu���X�V
static void GameUpdatePrintPos(void)
{
	const int	areaHeight = NUM_WINDOW_HEIGHT; 
	const int	areaWidth  = NUM_WINDOW_WIDTH / 2;
	int			sy, ey, sx, ex;	// �}�b�v�̕`��ʒu�I�t�Z�b�g
	int			py, px;			// �v���C���̕`��ʒu
	Position	playPos = {};
	playPos = m_manager->player->pos;

	const int say = NUM_HUMAN_HEIGHT / 2;
	const int sax = NUM_HUMAN_WIDTH / 2;

	// �v���C���[�̈ʒu����}�b�v�̕`��I�t�Z�b�g�ƁA�v���C���[�̕`��ʒu���l�� 
	// Y��
	if (playPos.y < areaHeight / 2 - say)// �v���C���[���E�B���h�E��[�ɂ���ꍇ
	{
		sy = 0;
		ey = areaHeight;
		py = playPos.y;
	}
	else if (playPos.y > (NUM_MAP_HEIGHT - areaHeight / 2) - say)// ���[
	{
		sy = NUM_MAP_HEIGHT - areaHeight;
		ey = NUM_MAP_HEIGHT;
		py = areaHeight / 2 + (areaHeight / 2 - (NUM_MAP_HEIGHT - playPos.y));
	}
	else// ����ȊO
	{
		sy = playPos.y - areaHeight / 2 + say;
		ey = playPos.y + areaHeight / 2 + say;
		py = areaHeight / 2 - say;
	}
	
	// X��
	if (playPos.x < areaWidth / 2 - sax){// ���[
	
		sx = 0;
		ex = areaWidth;
		px = playPos.x;
	}
	else if (playPos.x > (NUM_MAP_WIDTH - areaWidth / 2) - sax)//�E�[
	{
		sx = NUM_MAP_WIDTH - areaWidth;
		ex = NUM_MAP_WIDTH;
		px = areaWidth / 2 + (areaWidth / 2 - (NUM_MAP_WIDTH - playPos.x));
	}
	else// ����ȊO
	{
		sx = playPos.x - areaWidth / 2 + sax;
		ex = playPos.x + areaWidth / 2 + sax;
		px = areaWidth / 2 - sax;
	}
	// �l�������`��ʒu���Z�b�g
	m_manager->player->printPos.y = py;
	m_manager->player->printPos.x = px;
	MapSetPrintStartPos	(sy, sx);
	MapSetPrintEndPos	(ey, ex);
}

