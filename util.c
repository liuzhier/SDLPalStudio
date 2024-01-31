#include "main.h"

static char internal_buffer[PAL_MAX_GLOBAL_BUFFERS + 1][PAL_GLOBAL_BUFFER_SIZE];
#define INTERNAL_BUFFER_SIZE_ARGS internal_buffer[PAL_MAX_GLOBAL_BUFFERS], PAL_GLOBAL_BUFFER_SIZE

void
trim(
   char* str
)
/*++
  Purpose:

   Remove the leading and trailing spaces in a string.

  Parameters:

   str - the string to proceed.

  Return value:

   None.

--*/
{
   int pos = 0;
   char* dest = str;

   //
   // skip leading blanks
   //
   while (str[pos] <= ' ' && str[pos] > 0)
      pos++;

   while (str[pos])
   {
      *(dest++) = str[pos];
      pos++;
   }

   *(dest--) = '\0'; // store the null

   //
   // remove trailing blanks
   //
   while (dest >= str && *dest <= ' ' && *dest > 0)
      *(dest--) = '\0';
}

FILE*
UTIL_OpenRequiredFile(
   LPCSTR            lpszFileName
)
/*++
  Purpose:

   Open a required file. If fails, quit the program.
   打开所需的文件。如果失败，请退出程序。

  Parameters:

   [IN]  lpszFileName - file name to open.
   要打开的文件名。

  Return value:

   Pointer to the file.
   指向文件的指针。

--*/
{
   return UTIL_OpenRequiredFileForMode(lpszFileName, "rb");
}

FILE*
UTIL_OpenRequiredFileForMode(
   LPCSTR            lpszFileName,
   LPCSTR            szMode
)
/*++
  Purpose:

   Open a required file. If fails, quit the program.
   打开所需的文件。如果失败，请退出程序。

  Parameters:

   [IN]  lpszFileName - file name to open.
   要打开的文件名。
   [IN]  szMode - file open mode.
   文件打开模式。

  Return value:

   Pointer to the file.
   指向文件的指针。

--*/
{
   // 打开文件
   FILE* fp = UTIL_OpenFileForMode(lpszFileName, szMode);

   // 如果打开失败，重新打开文件
   if (fp == NULL)
   {
      fp = fopen(lpszFileName, szMode);
   }

   // 文件指针指向了不存在的文件，关闭程序
   if (fp == NULL)
   {
      //TerminateOnError("File open error(%d): %s!\n", errno, lpszFileName);
      printf("文件 %s 不存在！\n", lpszFileName);
   }

   return fp;
}

FILE*
UTIL_OpenFileForMode(
   LPCSTR            lpszFileName,
   LPCSTR            szMode
)
/*++
  Purpose:

   Open a file. If fails, return NULL.
   打开文件。如果失败，则返回NULL。

  Parameters:

   [IN]  lpszFileName - file name to open.
   要打开的文件名。
   [IN]  szMode - file open mode.
   文件打开模式。

  Return value:

   Pointer to the file.
   指向文件的指针。

--*/
{
   //
   // If lpszFileName is an absolute path, use its last element as filename
   // 如果lpszFileName是绝对路径，请使用其最后一个元素作为文件名
   if (UTIL_IsAbsolutePath(lpszFileName))
   {
      char* temp = strdup(lpszFileName), * filename = temp;
      FILE* fp = NULL;
      for (char* next = strpbrk(filename, PAL_PATH_SEPARATORS); next; next = strpbrk(filename = next + 1, PAL_PATH_SEPARATORS));
      if (*filename)
      {
         filename[-1] = '\0';
         fp = UTIL_OpenFileAtPathForMode(*temp ? temp : "/", filename, szMode);
      }
      free(temp);
      return fp;
   }

   return UTIL_OpenFileAtPathForMode(lpszPalPath, lpszFileName, szMode);
}

FILE*
UTIL_OpenFileAtPathForMode(
   LPCSTR              lpszPath,
   LPCSTR              lpszFileName,
   LPCSTR              szMode
)
{
   if (!lpszPath || !lpszFileName || !szMode) return NULL;

   //
   // Construct full path according to lpszPath and lpszFileName
   // 根据lpszPath和lpszFileName构造完整路径
   const char* path = UTIL_GetFullPathName(INTERNAL_BUFFER_SIZE_ARGS, lpszPath, lpszFileName);

   //
   // If no matching path, check the open mode
   // 如果没有匹配的路径，请检查打开模式
   if (path)
   {
      return fopen(path, szMode);
   }
   else if (szMode[0] != 'r')
   {
      return fopen(UTIL_CombinePath(INTERNAL_BUFFER_SIZE_ARGS, 2, lpszPath, lpszFileName), szMode);
   }
   else
   {
      return NULL;
   }
}

const char*
UTIL_GetFullPathName(
   char* buffer,
   size_t      buflen,
   const char* basepath,
   const char* subpath
)
{
   if (!buffer || !basepath || !subpath || buflen == 0) return NULL;

   int sublen = strlen(subpath);
   if (sublen == 0) return NULL;

   char* _base = strdup(basepath), * _sub = strdup(subpath);
   const char* result = NULL;

   if (access(UTIL_CombinePath(INTERNAL_BUFFER_SIZE_ARGS, 2, _base, _sub), 0) == 0)
   {
      result = internal_buffer[PAL_MAX_GLOBAL_BUFFERS];
   }

#ifndef __EMSCRIPTEN__
#if !defined(PAL_FILESYSTEM_IGNORE_CASE) || !PAL_FILESYSTEM_IGNORE_CASE
   if (result == NULL)
   {
      size_t pos = strspn(_sub, PAL_PATH_SEPARATORS);

      if (pos < sublen)
      {
         char* start = _sub + pos;
         char* end = strpbrk(start, PAL_PATH_SEPARATORS);
         if (end) *end = '\0';

         //
         // try to find the matching file in the directory.
         // 尝试在目录中找到匹配的文件
         struct dirent** list;
         int n = scandir(_base, &list, 0, alphasort);
         while (n-- > 0)
         {
            if (!result && SDL_strcasecmp(list[n]->d_name, start) == 0)
            {
               result = UTIL_CombinePath(INTERNAL_BUFFER_SIZE_ARGS, 2, _base, list[n]->d_name);
               if (end)
                  result = UTIL_GetFullPathName(INTERNAL_BUFFER_SIZE_ARGS, result, end + 1);
               else if (access(result, 0) != 0)
                  result = NULL;
            }
            free(list[n]);
         }
         free(list);
      }
   }
#endif
#endif
   if (result != NULL)
   {
      size_t dstlen = min(buflen - 1, strlen(result));
      result = (char*)memmove(buffer, result, dstlen);
      buffer[dstlen] = '\0';
   }

   free(_base);
   free(_sub);

   return result;
}

const char*
UTIL_CombinePath(
   char* buffer,
   size_t      buflen,
   int         numentry,
   ...
)
{
   if (buffer && buflen > 0 && numentry > 0)
   {
      const char* retval = buffer;
      va_list argptr;

      va_start(argptr, numentry);
      for (int i = 0; i < numentry && buflen > 1; i++)
      {
         const char* path = va_arg(argptr, const char*);
         int path_len = path ? strlen(path) : 0;
         int append_delim = (i < numentry - 1 && path_len > 0 && !PAL_IS_PATH_SEPARATOR(path[path_len - 1]));

         for (int is_sep = 0, j = 0; j < path_len && buflen >(size_t)append_delim + 1; j++)
         {
            //
            // Skip continuous path separators
            // 跳过连续路径分隔符
            if (PAL_IS_PATH_SEPARATOR(path[j]))
            {
               if (is_sep)
                  continue;
               else
                  is_sep = 1;
            }
            else
            {
               is_sep = 0;
            }
            *buffer++ = path[j];
            buflen--;
         }
         //
         // Make sure a path delimeter is append to the destination if this is not the last entry
         //  如果这不是最后一个条目，请确保将路径delimeter附加到目标
         if (append_delim)
         {
            *buffer++ = PAL_PATH_SEPARATORS[0];
            buflen--;
         }
      }
      va_end(argptr);

      *buffer = '\0';

      return retval;
   }
   else
   {
      return NULL;
   }
}

VOID
UTIL_CloseFile(
   FILE* fp
)
/*++
  Purpose:

   Close a file.

  Parameters:

   [IN]  fp - file handle to be closed.

  Return value:

   None.

--*/
{
   if (fp != NULL)
   {
      fclose(fp);
   }
}

char*
UTIL_va(
   char* buffer,
   int         buflen,
   const char* format,
   ...
)
{
   if (buflen > 0 && buffer)
   {
      va_list     argptr;

      va_start(argptr, format);
      vsnprintf(buffer, buflen, format, argptr);
      va_end(argptr);

      return buffer;
   }
   else
   {
      return NULL;
   }
}

char*
UTIL_GlobalBuffer(
   int         index
)
{
   return (index >= 0 && index < PAL_MAX_GLOBAL_BUFFERS) ? internal_buffer[index] : NULL;
}

void
TerminateOnError(
   const char* fmt,
   ...
)
// This function terminates the game because of an error and
// prints the message string pointed to by fmt both in the
// console and in a messagebox.
{
   printf("%s\n", fmt);
}

void*
UTIL_malloc(
   size_t               buffer_size
)
{
   // handy wrapper for operations we always forget, like checking malloc's returned pointer.

   void* buffer;

   // first off, check if buffer size is valid
   if (buffer_size == 0)
      TerminateOnError("UTIL_malloc() called with invalid buffer size: %d\n", buffer_size);

   buffer = malloc(buffer_size); // allocate real memory space

   // last check, check if malloc call succeeded
   if (buffer == NULL)
      TerminateOnError("UTIL_malloc() failure for %d bytes (out of memory?)\n", buffer_size);

   return buffer; // nothing went wrong, so return buffer pointer
}

LPSTR
UTIL_DecToHex(
   INT iDec,
   LPSTR lpsHex,
   INT iLen
)
{
   INT i, j = iDec, k;

   lpsHex[iLen] = '\0';

   for (i = iLen - 1; i >= 0; i--)
   {
      k = j % 16;
      j = j / 16;

      lpsHex[i] = strHexData[k];
   }

   return lpsHex;
}

WORD
UTIL_HexStrToDec(
   LPSTR str
)
{
   LPSTR lpStr = UTIL_StrToUpper(str);

   int i, j = 0;

   for (i = strlen(lpStr) - 1; i >= 0; i--)
   {
      j += (0x10 - (strlen(strchr(strHexData, lpStr[i])) - 1)) * pow(16, (strlen(lpStr) - 1) - i);
   }

   return j;
}

LPSTR
UTIL_StrToUpper(
   LPSTR str
)
{
   char* orign = str;

   for (; *str != '\0'; str++)
      *str = toupper(*str);

   return orign;
}

LPSTR
UTIL_StrTrim(
   LPSTR a
)
{
   LPSTR p1, * p2;

   p1 = a;
   p2 = a + strlen(a) - 1;

   while (p1 <= p2 && *p1 == ' ') {
      p1++;
   }

   while (p2 >= p1 && *p2 == ' ') {
      p2--;
   }

   *(++p2) = '\0';

   return p1;
}

LPSTR
UTIL_Long2String(
   LPSTR buff,
   LONG value
)
{
   LONG tmp;
   CHAR tmpBuf[32] = { 0 };
   // p 指向临时数组的最后一个位置
   LPSTR p = &tmpBuf[sizeof(tmpBuf) - 1];

   while (value != 0)
   {
      tmp = value / 10;
      // 把一个数字转成 ASCII 码，放到 p 指向的位置。
      // 然后 p 往前移动一个位置。
      *--p = (CHAR)('0' + (value - tmp * 10));
      value = tmp;
   }


   // 把临时数组中的每个字符，复制到 buff 中。
   while (*p) *buff++ = *p++;

   return buff;
}

VOID
UTIL_SaveText(
   LPCSTR lpszPath,
   LPCSTR szMode,
   LPCSTR szBuffer,
   const LPCSTR lpszFileName,
   ...
)
{
   FILE* fpTextFile;

   // 创建目录
   mkdir(lpszPath);

   // 创建文本文件
   if ((fpTextFile = UTIL_OpenFileAtPathForMode(lpszPath, PAL_va(1, lpszFileName), szMode)) == NULL)
   {
      return;
   }

   // 将缓冲区内容写入文件
   fputs(szBuffer, fpTextFile);

   // 关闭文件
   fclose(fpTextFile);
}

/*++
VOID
UTIL_ReadSystemIDConfig(
   VOID
)
{
   FILE* fp;
   WORD  i, SrciptDecID;

   LPSTR strMessage;

   // 每行最大读取的字符数
   CHAR StrLine[128];

   // 欲取内容
   CHAR strContent[65];
   LPSTR lpLine = strContent;
   memset(lpLine, 0, sizeof(lpLine));

   // 是否含有子参数
   BOOL fIsHasParams = FALSE;

   // 拼接注解文档路径
   //sprintf(lpLine, "%s%s%s", lpszPalPath, lpszPalMassagesPath, common_files[i]);

   fp = UTIL_OpenRequiredFileForMode(lpLine, "r");

   // 逐行读取文档
   while (TRUE)
   {
      // 读取一行
      fgets(StrLine, 1024, fp);

      // 若文件结束则跳出循环
      if (feof(fp))
         break;

      // 若改行开头不为 '@' 则跳过该行
      if ('@' != StrLine[0])
         continue;

      // 清空垃圾内存，以空数据填充 StrLine2
      // 将指令号截取出来，放入 StrLine2
      memset(strContent, 0, sizeof(strContent));
      memcpy(strContent, StrLine + 1, 4);
      SrciptDecID = UTIL_HexStrToDec(lpLine);

      // 清空垃圾内存，以空数据填充 StrLine2
      // 去掉该行的换行符，去掉该行';'符后面的注释区，
      // 将剩下的注解部分放入 StrLine2
      memset(strContent, 0, sizeof(strContent));
      memcpy(strContent, StrLine + 7, sizeof(StrLine) - 7);
      lpLine = strtok(strContent, "\n");
      lpLine = strtok(strContent, ";");
      lpLine = UTIL_StrTrim(lpLine);
      //swprintf(lpLine, "%s ", lpLine);

      // 只有读取脚本注解文档时才会继续执行
      if (!fIsHasParams)
      {
         // 空格作为参数分隔符
         LPSTR p = " ";
         LPSTR ret = NULL;

         // 清空内存指针垃圾
         memset(gpGlobals->t.strScriptIDMessage[SrciptDecID], 0, sizeof(gpGlobals->t.strScriptIDMessage[SrciptDecID]));

         for (i = 0, ret = strtok(lpLine, p); i < 4; ret = strtok(NULL, p), i++) {
            if (ret == NULL)
               break;

            swprintf(gpGlobals->t.strScriptIDMessage[SrciptDecID][i], "%s", ret);
         }
      }
      else
      {
         swprintf(gpGlobals->t.strScriptIDMessage[SrciptDecID], "%s", lpLine);
      }

      printf("% s\n", lpLine);
   }

   // 关闭文件
   fclose(fp);
}
--*/
