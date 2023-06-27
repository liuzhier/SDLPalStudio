#include "main.h"

#include <stdio.h>

int main(
	int      argc,
	char* argv[]
)
{
	// 初始化游戏数据
	PAL_LoadGameData();

	printf("	游戏数据解档完成\n");

	scanf_s("%d", &iEndInputCharacters);
	return 0;
}
