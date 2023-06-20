#include "main.h"
#include <errno.h>
#include <wctype.h>

#define WORD_LENGTH      10

TEXTLIB         g_TextLib;

INT
PAL_InitText(
	VOID
)
/*++
  Purpose:

  Initialize the in-game texts.

  Parameters:

  None.

  Return value:

  0 = success.
  -1 = memory allocation error.

  --*/
{

	FILE* fpMsg, * fpWord;
	int         i;

	//
	// Open the message and word data files.
	//
	fpMsg = UTIL_OpenRequiredFile("m.msg");
	fpWord = UTIL_OpenRequiredFile("word.dat");

	//
	// See how many words we have
	//
	fseek(fpWord, 0, SEEK_END);
	i = ftell(fpWord);

	//
	// Each word has 10 bytes
	//
	g_TextLib.nWords = (i + (WORD_LENGTH - 1)) / WORD_LENGTH;

	//
	// Read the words
	//
	g_TextLib.lpWordBuf = (LPBYTE)malloc(i);
	if (g_TextLib.lpWordBuf == NULL)
	{
		fclose(fpWord);
		fclose(fpMsg);
		return -1;
	}
	fseek(fpWord, 0, SEEK_SET);
	fread(g_TextLib.lpWordBuf, i, 1, fpWord);

	//
	// Close the words file
	//
	fclose(fpWord);

	//
	// Read the message offsets. The message offsets are in SSS.MKF #3
	//
	i = PAL_MKFGetChunkSize(3, gpGlobals->f.fpSSS) / sizeof(DWORD);
	g_TextLib.nMsgs = i - 1;

	g_TextLib.lpMsgOffset = (LPDWORD)malloc(i * sizeof(DWORD));
	if (g_TextLib.lpMsgOffset == NULL)
	{
		free(g_TextLib.lpWordBuf);
		fclose(fpMsg);
		return -1;
	}

	PAL_MKFReadChunk((LPBYTE)(g_TextLib.lpMsgOffset), i * sizeof(DWORD), 3,
		gpGlobals->f.fpSSS);

	//
	// Read the messages.
	//
	fseek(fpMsg, 0, SEEK_END);
	i = ftell(fpMsg);

	g_TextLib.lpMsgBuf = (LPBYTE)malloc(i);
	if (g_TextLib.lpMsgBuf == NULL)
	{
		free(g_TextLib.lpMsgOffset);
		free(g_TextLib.lpWordBuf);
		fclose(fpMsg);
		return -1;
	}

	fseek(fpMsg, 0, SEEK_SET);
	fread(g_TextLib.lpMsgBuf, 1, i, fpMsg);

	fclose(fpMsg);

	return 0;
}

VOID
PAL_FreeText(
	VOID
)
/*++
  Purpose:

	Free the memory used by the texts.

  Parameters:

	None.

  Return value:

	None.

--*/
{
	if (g_TextLib.lpMsgBuf != NULL)
	{
		free(g_TextLib.lpMsgBuf);
		g_TextLib.lpMsgBuf = NULL;
	}

	if (g_TextLib.lpMsgOffset != NULL)
	{
		free(g_TextLib.lpMsgOffset);
		g_TextLib.lpMsgOffset = NULL;
	}

	if (g_TextLib.lpWordBuf != NULL)
	{
		free(g_TextLib.lpWordBuf);
		g_TextLib.lpWordBuf = NULL;
	}
}

LPCSTR
PAL_BIG5ToGB(
	LPCSTR     lpszBIG5String
)
{

}

LPCSTR
PAL_GetWord(
	WORD        wNumWord
)
/*++
  Purpose:

  Get the specified word.

  Parameters:

  [IN]  wNumWord - the number of the requested word.

  Return value:

  Pointer to the requested word. NULL if not found.

  --*/
{
	static char buf[WORD_LENGTH + 1];

	if (wNumWord >= g_TextLib.nWords)
	{
		return NULL;
	}

	memcpy(buf, &g_TextLib.lpWordBuf[wNumWord * WORD_LENGTH], WORD_LENGTH);
	buf[WORD_LENGTH] = '\0';

	//
	// Remove the trailing spaces
	//
	trim(buf);

	if ((strlen(buf) & 1) != 0 && buf[strlen(buf) - 1] == '1')
	{
		buf[strlen(buf) - 1] = '\0';
	}

	return buf;
}

LPCSTR
PAL_GetMsg(
	WORD       wNumMsg
)
/*++
  Purpose:

  Get the specified message.

  Parameters:

  [IN]  wNumMsg - the number of the requested message.

  Return value:

  Pointer to the requested message. NULL if not found.

  --*/
{
	static char    buf[256];
	DWORD          dwOffset, dwSize;

	if (wNumMsg >= g_TextLib.nMsgs)
	{
		return NULL;
	}

	dwOffset = SWAP32(g_TextLib.lpMsgOffset[wNumMsg]);
	dwSize = SWAP32(g_TextLib.lpMsgOffset[wNumMsg + 1]) - dwOffset;
	assert(dwSize < 255);

	memcpy(buf, &g_TextLib.lpMsgBuf[dwOffset], dwSize);
	buf[dwSize] = '\0';

	return buf;
}
