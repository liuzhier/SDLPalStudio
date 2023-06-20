#ifndef UTIL_H
#define UTIL_H

#include "common.h"
#include "palcommon.h"

PAL_C_LINKAGE_BEGIN

void
trim(
	char* str
);

/*
 * Logging utilities
 */

 /*++
   Purpose:

	 The pointer to callback function that produces actual log output.

   Parameters:

	 [IN]  level    - The log level of this output call.
	 [IN]  full_log - The full log string produced by UTIL_LogOutput.
	 [IN]  user_log - The log string produced by user-provided format.

   Return value:

	 None.

 --*/
typedef void(*LOGCALLBACK)(const char* full_log, const char* user_log);

FILE*
UTIL_OpenRequiredFile(
	LPCSTR            lpszFileName
);

FILE*
UTIL_OpenRequiredFileForMode(
	LPCSTR            lpszFileName,
	LPCSTR            szMode
);

FILE*
UTIL_OpenFileForMode(
	LPCSTR            lpszFileName,
	LPCSTR            szMode
);

BOOL
UTIL_IsAbsolutePath(
	const char* lpszFileName
);

FILE*
UTIL_OpenFileAtPathForMode(
	LPCSTR              lpszPath,
	LPCSTR              lpszFileName,
	LPCSTR              szMode
);

const char*
UTIL_GetFullPathName(
	char* buffer,
	size_t      buflen,
	const char* basepath,
	const char* subpath
);

/*++
  Purpose:

	Combine the 'dir' and 'file' part into a single path string.
	If 'dir' is non-NULL, then it ensures that the output string contains
	'/' between 'dir' and 'file' (no matter whether 'file' is NULL or not).

  Parameters:

	buffer - user-supplied buffer.
	buflen - size of the buffer, including null-terminator.
	dir    - the directory path.
	file   - the file path.

  Return value:

	The value of buffer if buffer is non-NULL and buflen > 0, otherwise NULL.

--*/
const char*
UTIL_CombinePath(
	char* buffer,
	size_t      buflen,
	int         numentry,
	...
);
#define PAL_CombinePath(i, d, f) UTIL_CombinePath(UTIL_GlobalBuffer(i), PAL_GLOBAL_BUFFER_SIZE, 2, (d), (f))

VOID
UTIL_CloseFile(
	FILE* fp
);

/*++
  Purpose:

	Does a varargs printf into the user-supplied buffer,
	so we don't need to have varargs versions of all text functions.

  Parameters:

	buffer - user-supplied buffer.
	buflen - size of the buffer, including null-terminator.
	format - the format string.

  Return value:

	The value of buffer if buffer is non-NULL and buflen > 0, otherwise NULL.

--*/
char*
UTIL_va(
	char* buffer,
	int         buflen,
	const char* format,
	...
);
#define PAL_va(i, fmt, ...) UTIL_va(UTIL_GlobalBuffer(i), PAL_GLOBAL_BUFFER_SIZE, fmt, __VA_ARGS__)

char*
UTIL_GlobalBuffer(
	int         index
);

void
TerminateOnError(
	const char* fmt,
	...
);

void*
UTIL_malloc(
	size_t               buffer_size
);

LPSTR
UTIL_DecToHex(
	INT iDec,
	LPSTR lpsHex,
	INT iLen
);

WORD
UTIL_HexStrToDec(
	LPSTR str
);

LPSTR
UTIL_StrToUpper(
	LPSTR str
);

LPSTR
UTIL_StrTrim(
	LPSTR a
);

LPSTR
UTIL_Long2String(
	LPSTR buff,
	LONG value
);

LPSTR
UTIL_Long2String(
	LPSTR buff,
	LONG value
);

VOID
UTIL_SaveText(
	LPCSTR              lpszPath,
	LPCSTR              lpszFileName,
	LPCSTR              szMode,
	LPCSTR              szBuffer,
	...
);
#define UTIL_SaveTextFile(lpszPath, szMode, szBuffer, fmt, ...) UTIL_SaveText(lpszPath, szMode, szBuffer, fmt, __VA_ARGS__);

PAL_C_LINKAGE_END

#endif
