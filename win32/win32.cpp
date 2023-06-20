#include "../global.h"
#include "../util.h"

extern "C"
BOOL
UTIL_IsAbsolutePath(
	LPCSTR  lpszFileName
)
{
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFname[_MAX_FNAME], szExt[_MAX_EXT];
	if (lpszFileName == NULL)
		return FALSE;
#if !defined(__MINGW32__) // MinGW Distro's win32 api lacks this...Anyway, winxp lacks this too
	if (_splitpath_s(lpszFileName, szDrive, szDir, szFname, szExt) == 0)
#else
	_splitpath(lpszFileName, szDrive, szDir, szFname, szExt);
	if (errno != EINVAL)
#endif
		return (strlen(szDrive) > 0 && (szDir[0] == '\\' || szDir[0] == '/'));
	else
		return FALSE;
}
