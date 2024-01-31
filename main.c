#include "main.h"

#include <stdio.h>

int
main(
   int      argc,
   char* argv[]
)
{
   // 初始化全局数据
   PAL_InitGlobals();

   // 初始化游戏数据
   PAL_InitGlobalGameData();

   // 创建工作目录
   mkdir(lpszPalMassagesPath);

   //
   // SSS
   //
   // 解档场景和事件
   PAL_SaveGameScene();
   //
   // 解档对象......
   PAL_SaveGameObject();
   //
   // 解档脚本内容
   PAL_SaveGameScript();

   //
   // DATA
   //
   // 解档商店
   PAL_SaveGameBaseStore();
   //
   // 解档敌方基数据
   PAL_SaveGameBaseEnemy();
   //
   // 解档敌方队列
   PAL_SaveGameBaseEnemyTeam();
   //
   // 解档仙术基数据
   PAL_SaveGameBaseMagic();
   //
   // 解档战场对灵抗的影响以及战场背景波动
   PAL_SaveGameBaseBattleField();
   //
   // 解档领悟仙术所需修行
   PAL_SaveGameBaseLevelUpMagic();
   //
   // 解档我方施法特效具体设定
   PAL_SaveGameBaseBattleEffectIndex();
   //
   // 解档战场中敌人数量对应坐标
   PAL_SaveGameBaseEnemyPos();
   //
   // 解档我方修行晋所需经历
   PAL_SaveGameBaseLevelUpExp();

   // 释放全局数据
   PAL_FreeGlobals();

   printf("	游戏数据解档完成\n");

   scanf_s("%d", &iEndInputCharacters);
   return 0;
}
