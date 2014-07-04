#include "Effect.h"

/*-----------------------------------------------------------------------------
  Effect: Static Numbers
  ---------------------------------------------------------------------------*/
enum {
	MSGSIZE = 32,	
};

/*-----------------------------------------------------------------------------
  Effect: Static Typedef
  ---------------------------------------------------------------------------*/
typedef struct tagEffectData {
	char msg[MSGSIZE];
	int y,
		x,
		fgColor,
		bgColor;
} EffectData;


/*-----------------------------------------------------------------------------
  Effect: Static Variables
  ---------------------------------------------------------------------------*/
static ListEntry m_list = { sizeof(EffectData), };

/*-----------------------------------------------------------------------------
  Effect: Extern Functions
  ---------------------------------------------------------------------------*/
extern void EffectPopupMessage(const char *msg, int y, int x, int fgColor, int bgColor)
{
	if (y < 0 || x < 0)
		return;
	
	EffectData data = {};
	
	snprintf(data.msg, sizeof(data.msg), "%s", msg);
	data.y = y;
	data.x = x;
	data.fgColor = fgColor;
	data.bgColor = bgColor;
	
	List_PushBack(&m_list, &data);
}

extern void EffectPrint()
{
	EffectData *data;
	
	while (! List_Empty(&m_list))
	{
		data = (EffectData *)List_Back(&m_list);
		ConsolePrintFormat(data->y, data->x, data->fgColor, data->bgColor, data->msg);
		
		List_PopBack(&m_list);
	}
}

extern void EffectCleanup()
{
	List_Clear(&m_list);
}

