#include "KeyInput.h"

/*-----------------------------------------------------------------------------
  KeyInput: Static Variables
  ---------------------------------------------------------------------------*/
static int m_key;

/*-----------------------------------------------------------------------------
  KeyInput: Extern Functions
  ---------------------------------------------------------------------------*/
extern int KeyWait(void)
{
	m_key = 0;
    do {
       m_key += _getch();
    } while (m_key == 224);

	switch (m_key) {
		case	224 + 72	: m_key = NUM_KEY_UP;	    break;/* Cursor UP */
		case	224 + 77	: m_key = NUM_KEY_RIGHT;	break;/* Cursor RIGHT */
		case	224 + 80	: m_key = NUM_KEY_DOWN;		break;/* Cursor DOWN */
		case	224 + 75	: m_key = NUM_KEY_LEFT;		break;/* Cursor LEFT */

		case	224 + 141	: m_key = NUM_KEY_GRUB_UP;		break;
		case	224 + 116	: m_key = NUM_KEY_GRUB_RIGHT;	break;
		case	224 + 145	: m_key = NUM_KEY_GRUB_DOWN;	break;
		case	224 + 115	: m_key = NUM_KEY_GRUB_LEFT;	break;

		case   'E'	: m_key = NUM_KEY_EXIT;		break;
		case	'a'	: m_key = NUM_KEY_ATTACK;	break;
		case	27	: m_key = NUM_KEY_MENU;		break;/* ESC */
		case	' ' : m_key = NUM_KEY_STAGE_CHANGE; break;/* debug: ゲームの初期化 */
		case	'1' : m_key = NUM_KEY_DEBUG_1;		break;/* debug: ステージに３を加算してゲームリスタート */
		case 	'2' : m_key = NUM_KEY_DEBUG_MAXSTAGE_BEFORE; break;/* debug: ゲーム最終ステージのひとつ前までジャンプ。*/
		default		: m_key = NUM_KEY_INVALID;	break;
	}
	return m_key;
}

/** m_keyに引数の数値が格納されていたら、1を返す。
 * @param	keyValue
 */
extern int KeyPressed(int keyValue)
{
	if (m_key == keyValue)
		return 1;
	else
		return 0;
}

extern int KeyGet(void)
{
	return m_key;
}

