#include "palcommon.h"
#include "global.h"

INT
PAL_MKFGetChunkCount(
	FILE* fp
)
/*++
  Purpose:

	Get the number of chunks in an MKF archive.
	获取MKF存档中的块数。

  Parameters:

	[IN]  fp - pointer to an fopen'ed MKF file.
	指向已打开的MKF文件的指针。

  Return value:

	Integer value which indicates the number of chunks in the specified MKF file.
	指定的MKF文件中块数的整数值。

--*/
{
	INT iNumChunk;
	if (fp == NULL)
	{
		return 0;
	}

	//
	// 设置文件读取偏移为文件种第 0 个字节
	//
	fseek(fp, 0, SEEK_SET);

	//
	// 一般的MKF在开头第一个字节的值文件头数据库的长度。文件头中，有若干数据块
	// 来代表每个 smkf 的指针，每个数据块占4个字节，就是一个 INT 类型数据的大小，
	// 最后会多一个空的 smkf 的指针，文件头数据块数与 smkf 数据块数相同。
	// 
	// 求文件头数据块数，以
	// (第一个数据块指针的值 - 最后一个空数据块指针的长度) / 每个数据块指针的长度
	// 下面的 >> 2 是二进制的操作,进制中，右移两位相当于除以了该进制位权的 2 倍
	// 与除以一个INT类型数据的大小一样，fread函数返回值 0 和 1，表示 TRUE 和 FALSE
	//
	if (fread(&iNumChunk, sizeof(INT), 1, fp) == 1)
		return (SDL_SwapLE32(iNumChunk) - 4) >> 2;
	else
		return 0;
}

INT
PAL_MKFReadChunk(
	LPBYTE          lpBuffer,
	UINT            uiBufferSize,
	UINT            uiChunkNum,
	FILE* fp
)
/*++
  Purpose:

	Read a chunk from an MKF archive into lpBuffer.
	将MKF归档文件中的块读取到lpBuffer中。

  Parameters:

	[OUT] lpBuffer - pointer to the destination buffer.
	指向目标缓冲区的指针。

	[IN]  uiBufferSize - size of the destination buffer.
	目标缓冲区的大小。

	[IN]  uiChunkNum - the number of the chunk in the MKF archive to read.
	MKF存档文件中要读取的区块数。

	[IN]  fp - pointer to the fopen'ed MKF file.
	指向已打开的MKF文件的指针。

  Return value:

	Integer value which indicates the size of the chunk.
	-1 if there are error in parameters.
	-2 if buffer size is not enough.
	指示块大小的整数值。
	-1  若参数有错误。
	-2，若缓冲区大小不够。

--*/
{
	UINT     uiOffset = 0;
	UINT     uiNextOffset = 0;
	UINT     uiChunkCount;
	UINT     uiChunkLen;

	if (lpBuffer == NULL || fp == NULL || uiBufferSize == 0)
	{
		return -1;
	}

	//
	// Get the total number of chunks.
	// 获取块的总数。
	uiChunkCount = PAL_MKFGetChunkCount(fp);
	if (uiChunkNum >= uiChunkCount)
	{
		return -1;
	}

	//
	// Get the offset of the chunk.
	// 获取块的偏移量。
	fseek(fp, 4 * uiChunkNum, SEEK_SET);
	PAL_fread(&uiOffset, 4, 1, fp);
	PAL_fread(&uiNextOffset, 4, 1, fp);
	uiOffset = SDL_SwapLE32(uiOffset);
	uiNextOffset = SDL_SwapLE32(uiNextOffset);

	//
	// Get the length of the chunk.
	// 获取块的长度。
	uiChunkLen = uiNextOffset - uiOffset;

	if (uiChunkLen > uiBufferSize)
	{
		return -2;
	}

	if (uiChunkLen != 0)
	{
		fseek(fp, uiOffset, SEEK_SET);
		return (int)fread(lpBuffer, 1, uiChunkLen, fp);
	}

	return -1;
}

INT
PAL_MKFGetChunkSize(
	UINT    uiChunkNum,
	FILE* fp
)
/*++
  Purpose:

	Get the size of a chunk in an MKF archive.
	获取MKF存档中指定块的大小。

  Parameters:

	[IN]  uiChunkNum - the number of the chunk in the MKF archive.
	MKF存档中块的编号。


	[IN]  fp - pointer to the fopen'ed MKF file.
	指向已打开的MKF文件的指针。


  Return value:

	Integer value which indicates the size of the chunk.
	-1 if the chunk does not exist.
	指示块大小的整数值。
	-1  若指定的块不存在。

--*/
{
	UINT    uiOffset = 0;
	UINT    uiNextOffset = 0;
	UINT    uiChunkCount = 0;

	//
	// Get the total number of chunks.
	// 获取块的总数。
	uiChunkCount = PAL_MKFGetChunkCount(fp);
	if (uiChunkNum >= uiChunkCount)
	{
		return -1;
	}

	//
	// Get the offset of the specified chunk and the next chunk.
	// 获取指定块和下一块的偏移量。
	fseek(fp, 4 * uiChunkNum, SEEK_SET);
	PAL_fread(&uiOffset, sizeof(UINT), 1, fp);
	PAL_fread(&uiNextOffset, sizeof(UINT), 1, fp);

	uiOffset = SDL_SwapLE32(uiOffset);
	uiNextOffset = SDL_SwapLE32(uiNextOffset);

	//
	// Return the length of the chunk.
	// 返回块的长度。
	return uiNextOffset - uiOffset;
}
