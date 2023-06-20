#ifndef _COMMON_H
#define _COMMON_H

#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SDL.h"
#include "SDL_endian.h"

#include "pal_config.h"

# define TRUE                1
# define FALSE               0
# define VOID                void

typedef char                CHAR;
typedef wchar_t             WCHAR;
typedef short               SHORT;
typedef long                LONG;

typedef unsigned long       ULONG, * PULONG;
typedef unsigned short      USHORT, * PUSHORT;
typedef unsigned char       UCHAR, * PUCHAR;

typedef unsigned short      WORD, * LPWORD;
typedef unsigned int        DWORD, * LPDWORD;
typedef int                 INT, * LPINT;

typedef int                 BOOL, * LPBOOL;

typedef unsigned int        UINT, * PUINT, UINT32, * PUINT32;
typedef unsigned char       BYTE, * LPBYTE;
typedef const BYTE* LPCBYTE;
typedef float               FLOAT, * LPFLOAT;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef CHAR* LPSTR;
typedef const CHAR* LPCSTR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;

static CHAR strHexData[] = "0123456789ABCDEF";

//# define sprintf                snprintf

# define PAL_C_LINKAGE_BEGIN
# define PAL_C_LINKAGE_END

#define PAL_MAX_GLOBAL_BUFFERS 4
#define PAL_GLOBAL_BUFFER_SIZE 1024

#ifndef PAL_IS_PATH_SEPARATOR
# define PAL_IS_PATH_SEPARATOR(x) ((x) == '/')
#endif

// PAL_fread(存放数据的内存流，从当前文件指针往后读 elem 个字节，
// 读 num 个 elem 字节，文件指针)，将读到的内存交给 buf 指针
#define PAL_fread(buf, elem, num, fp) if (fread((buf), (elem), (num), (fp)) < (num)) return -1

#define PAL_empty(buf) !(LPSTR)(&buf)

# define strdup _strdup
# define access _access
# define itoa _itoa

#ifdef __cplusplus
# define PAL_C_LINKAGE       extern "C"
# define PAL_C_LINKAGE_BEGIN PAL_C_LINKAGE {
# define PAL_C_LINKAGE_END   }
#else
# define PAL_C_LINKAGE
# define PAL_C_LINKAGE_BEGIN
# define PAL_C_LINKAGE_END
#endif

#define SWAP32(X)    (X)

#endif    //_COMMON_H
