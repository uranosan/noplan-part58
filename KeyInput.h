#ifndef __KEY_INPUT_H_INCLUDED__
#define __KEY_INPUT_H_INCLUDED__

#include <conio.h>

/*-----------------------------------------------------------------------------
  KeyInput: Global Numbers
  ---------------------------------------------------------------------------*/
enum TAG_NUM_KEY_VALUE {
	NUM_KEY_EXIT		= -1,
	NUM_KEY_INVALID	= 0,
	
	NUM_KEY_WRAP_START,	/* キーラップ開始 */
	NUM_KEY_UP,
	NUM_KEY_RIGHT,
	NUM_KEY_DOWN,
	NUM_KEY_LEFT,
	NUM_KEY_ATTACK,
	NUM_KEY_GRUB_UP,
	NUM_KEY_GRUB_RIGHT,
	NUM_KEY_GRUB_DOWN,
	NUM_KEY_GRUB_LEFT,
	NUM_KEY_WRAP_END,	/* キーラップ終了 */
	
	NUM_KEY_MENU,

	NUM_KEY_STAGE_CHANGE,/* debug */
	NUM_KEY_DEBUG_1,/* debug */
	NUM_KEY_DEBUG_MAXSTAGE_BEFORE,/* debug */
};

/*-----------------------------------------------------------------------------
  KeyInput: Prototype Extern Functions
  ---------------------------------------------------------------------------*/
extern int KeyWait(void);
extern int KeyPressed(int keyValue);
extern int KeyGet(void);

#endif/*__KEY_INPUT_H_INCLUDED__*/

