#ifndef _TEXT_H
#define _TEXT_H

#include "common.h"

typedef struct tagTEXTLIB
{
	LPBYTE          lpWordBuf;
	LPBYTE          lpMsgBuf;
	LPDWORD         lpMsgOffset;
	int*** lpIndexBuf;

	int             nWords;
	int             nMsgs;
} TEXTLIB, * LPTEXTLIB;

extern TEXTLIB         g_TextLib;

INT
PAL_InitText(
	VOID
);

VOID
PAL_FreeText(
	VOID
);

LPCSTR
PAL_BIG5ToGB(
	LPCSTR     lpszBIG5String
);

LPCSTR
PAL_GetWord(
	WORD       wNumWord
);

LPCSTR
PAL_GetMsg(
	WORD       wNumMsg
);

#endif // _TEXT_H