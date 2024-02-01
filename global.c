#include "main.h"

static GLOBALVARS _gGlobals;
GLOBALVARS* const gpGlobals = &_gGlobals;

const static LPSTR* strScriptType[sizeof(enum tagSCRIPTTYPE)] = { "OnEnter", "OnTeleport" };
const static LPSTR strZeroBug = "@FFFF: 0000 0000 0000 0000 : 停止执行";

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define DO_BYTESWAP(buf, size)
#else
#define DO_BYTESWAP(buf, size)                                   \
   do {                                                          \
      int i;                                                     \
      for (i = 0; i < (size) / 2; i++)                           \
      {                                                          \
         ((LPWORD)(buf))[i] = SDL_SwapLE16(((LPWORD)(buf))[i]);  \
      }                                                          \
   } while(0)
#endif

// 预定义：直接加载文件数据
#define LOAD_DATA(buf, size, chunknum, fp)                       \
   do {                                                          \
      PAL_MKFReadChunk((LPBYTE)(buf), (size), (chunknum), (fp)); \
      DO_BYTESWAP(buf, size);                                    \
   } while(0)

BOOL
PAL_IsWINVersion(
   VOID
)
{
   FILE* fps[] = { UTIL_OpenRequiredFile("abc.mkf"), UTIL_OpenRequiredFile("map.mkf"), gpGlobals->f.fpF, gpGlobals->f.fpFBP, gpGlobals->f.fpFIRE, gpGlobals->f.fpMGO };
   uint8_t* data = NULL;
   int data_size = 0, dos_score = 0, win_score = 0;
   BOOL result = FALSE;

   for (int i = 0; i < sizeof(fps) / sizeof(FILE*); i++)
   {
      //
      // Find the first non-empty sub-file
      //
      int count = PAL_MKFGetChunkCount(fps[i]), j = 0, size;
      while (j < count && (size = PAL_MKFGetChunkSize(j, fps[i])) < 4) j++;
      if (j >= count) goto PAL_IsWINVersion_Exit;

      //
      // Read the content and check the compression signature
      // Note that this check is not 100% correct, however in incorrect situations,
      // the sub-file will be over 784MB if uncompressed, which is highly unlikely.
      //
      if (data_size < size) data = (uint8_t*)realloc(data, data_size = size);
      PAL_MKFReadChunk(data, data_size, j, fps[i]);
      if (data[0] == 'Y' && data[1] == 'J' && data[2] == '_' && data[3] == '1')
      {
         if (win_score > 0)
            goto PAL_IsWINVersion_Exit;
         else
            dos_score++;
      }
      else
      {
         if (dos_score > 0)
            goto PAL_IsWINVersion_Exit;
         else
            win_score++;
      }
   }

   //
   // Finally check the size of object definition
   //
   data_size = PAL_MKFGetChunkSize(2, gpGlobals->f.fpSSS);
   if (data_size % sizeof(OBJECT) == 0 && data_size % sizeof(OBJECT_DOS) != 0 && dos_score > 0) goto PAL_IsWINVersion_Exit;
   if (data_size % sizeof(OBJECT_DOS) == 0 && data_size % sizeof(OBJECT) != 0 && win_score > 0) goto PAL_IsWINVersion_Exit;

   //if (fIsWIN95)
   gpGlobals->fIsWIN95 = (win_score == sizeof(fps) / sizeof(FILE*)) ? TRUE : FALSE;

   result = TRUE;

PAL_IsWINVersion_Exit:
   free(data);
   fclose(fps[1]);
   fclose(fps[0]);

   return result;
}

static VOID
PAL_ReadGlobalGameData(
   VOID
)
/*++
  Purpose:

   Read global game data from data files.

  Parameters:

   None.

  Return value:

   None.

--*/
{
   const GAMEDATA* p = &gpGlobals->g;

   // 读取事件
   LOAD_DATA(p->lprgEventObject, p->nEventObject * sizeof(EVENTOBJECT),
      0, gpGlobals->f.fpSSS);

   // 读取场景
   PAL_MKFReadChunk((LPBYTE)(p->rgScene), sizeof(p->rgScene), 1, gpGlobals->f.fpSSS);
   DO_BYTESWAP(p->rgScene, sizeof(p->rgScene));

   // 读取脚本
   LOAD_DATA(p->lprgScriptEntry, p->nScriptEntry * sizeof(SCRIPTENTRY),
      4, gpGlobals->f.fpSSS);

   LOAD_DATA(p->lprgStore, p->nStore * sizeof(STORE), 0, gpGlobals->f.fpDATA);
   LOAD_DATA(p->lprgEnemy, p->nEnemy * sizeof(ENEMY), 1, gpGlobals->f.fpDATA);
   LOAD_DATA(p->lprgEnemyTeam, p->nEnemyTeam * sizeof(ENEMYTEAM),
      2, gpGlobals->f.fpDATA);
   LOAD_DATA(p->lprgMagic, p->nMagic * sizeof(MAGIC), 4, gpGlobals->f.fpDATA);
   LOAD_DATA(p->lprgBattleField, p->nBattleField * sizeof(BATTLEFIELD),
      5, gpGlobals->f.fpDATA);
   LOAD_DATA(p->lprgLevelUpMagic, p->nLevelUpMagic * sizeof(LEVELUPMAGIC_ALL),
      6, gpGlobals->f.fpDATA);
   LOAD_DATA(p->rgwBattleEffectIndex, sizeof(p->rgwBattleEffectIndex),
      11, gpGlobals->f.fpDATA);
   PAL_MKFReadChunk((LPBYTE) & (p->EnemyPos), sizeof(p->EnemyPos),
      13, gpGlobals->f.fpDATA);
   DO_BYTESWAP(&(p->EnemyPos), sizeof(p->EnemyPos));
   PAL_MKFReadChunk((LPBYTE)(p->rgLevelUpExp), sizeof(p->rgLevelUpExp),
      14, gpGlobals->f.fpDATA);
   DO_BYTESWAP(p->rgLevelUpExp, sizeof(p->rgLevelUpExp));
}

static VOID
PAL_LoadDefaultGame(
   VOID
)
/*++
  Purpose:

   Load the default game data.

  Parameters:

   None.

  Return value:

   None.

--*/
{
   GAMEDATA* p = &gpGlobals->g;
   UINT32       i;

   //
   // Load the default data from the game data files.
   // 新的故事，读取初始场景、事件、队员数据
   LOAD_DATA(p->lprgEventObject, p->nEventObject * sizeof(EVENTOBJECT),
      0, gpGlobals->f.fpSSS);
   PAL_MKFReadChunk((LPBYTE)(p->rgScene), sizeof(p->rgScene), 1, gpGlobals->f.fpSSS);
   DO_BYTESWAP(p->rgScene, sizeof(p->rgScene));
   if (gpGlobals->fIsWIN95)
   {
      PAL_MKFReadChunk((LPBYTE)(p->rgObject), sizeof(p->rgObject), 2, gpGlobals->f.fpSSS);
      DO_BYTESWAP(p->rgObject, sizeof(p->rgObject));
   }
   else
   {
      OBJECT_DOS objects[MAX_OBJECTS];
      PAL_MKFReadChunk((LPBYTE)(objects), sizeof(objects), 2, gpGlobals->f.fpSSS);
      DO_BYTESWAP(objects, sizeof(objects));
      //
      // Convert the DOS-style data structure to WIN-style data structure
      //
      for (i = 0; i < MAX_OBJECTS; i++)
      {
         memcpy(&p->rgObject[i], &objects[i], sizeof(OBJECT_DOS));
         p->rgObject[i].rgwData[6] = objects[i].rgwData[5];     // wFlags
         p->rgObject[i].rgwData[5] = 0;                         // wScriptDesc or wReserved2
      }
   }

   PAL_MKFReadChunk((LPBYTE)(&(p->PlayerRoles)), sizeof(PLAYERROLES),
      3, gpGlobals->f.fpDATA);
   DO_BYTESWAP(&(p->PlayerRoles), sizeof(PLAYERROLES));
}

VOID
PAL_InitGlobalGameData(
   VOID
)
/*++
  Purpose:

   Initialize global game data.

  Parameters:

   None.

  Return value:

   None.

--*/
{
   int        len;

#define PAL_DOALLOCATE(fp, num, type, lptype, ptr, n)                            \
   {                                                                             \
      len = PAL_MKFGetChunkSize(num, fp);                                        \
      ptr = (lptype)malloc(len);                                                 \
      n = len / sizeof(type);                                                    \
      if (ptr == NULL)                                                           \
      {                                                                          \
         TerminateOnError("PAL_InitGlobalGameData(): Memory allocation error!"); \
	  }                                                                          \
	}

   //
   // If the memory has not been allocated, allocate first.
   // 如果内存尚未分配，将先分配。
   if (gpGlobals->g.lprgEventObject == NULL)
   {
      PAL_DOALLOCATE(gpGlobals->f.fpSSS, 0, EVENTOBJECT, LPEVENTOBJECT,
         gpGlobals->g.lprgEventObject, gpGlobals->g.nEventObject);

      PAL_DOALLOCATE(gpGlobals->f.fpSSS, 4, SCRIPTENTRY, LPSCRIPTENTRY,
         gpGlobals->g.lprgScriptEntry, gpGlobals->g.nScriptEntry);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 0, STORE, LPSTORE,
         gpGlobals->g.lprgStore, gpGlobals->g.nStore);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 1, ENEMY, LPENEMY,
         gpGlobals->g.lprgEnemy, gpGlobals->g.nEnemy);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 2, ENEMYTEAM, LPENEMYTEAM,
         gpGlobals->g.lprgEnemyTeam, gpGlobals->g.nEnemyTeam);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 4, MAGIC, LPMAGIC,
         gpGlobals->g.lprgMagic, gpGlobals->g.nMagic);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 5, BATTLEFIELD, LPBATTLEFIELD,
         gpGlobals->g.lprgBattleField, gpGlobals->g.nBattleField);

      PAL_DOALLOCATE(gpGlobals->f.fpDATA, 6, LEVELUPMAGIC_ALL, LPLEVELUPMAGIC_ALL,
         gpGlobals->g.lprgLevelUpMagic, gpGlobals->g.nLevelUpMagic);

      PAL_ReadGlobalGameData();
   }
#undef PAL_DOALLOCATE

   // 初始化对话和对象名称
   PAL_InitText();

   //
   // Cannot load the saved game file. Load the defaults.
   //
   PAL_LoadDefaultGame();
}

#ifdef DEBUG_DO_WHILE_PAL_SaveGameScript
static VOID
_PAL_SaveGameScript(
   WORD wScrpitEntry,
   LPSTR strScriptSavePath,
   WORD wScriptType,
   WORD wRoundOfJump
)
{
   GAMEDATA* p = &gpGlobals->g;

   WORD wIndex, wScriptAddress = wScrpitEntry, wScriptAddressLast = 0;
   INT i = 0, iJumpLen = 0;

   LPSCRIPTENTRY pScript = &(*p).lprgScriptEntry[wScriptAddress];

   WORD wOperation, wThisJumpRound = 0;

   BOOL fEnded = FALSE, fScriptJump = FALSE;

   CHAR strScrpitEntryLast[65000];
   CHAR strScrpitEntry[65000];
   CHAR strToHex[5];
   CHAR strScriptMessage[256];
   LPSTR lpScrpitEntryLast = strScrpitEntryLast;
   LPSTR lpScrpitEntry = strScrpitEntry;
   LPSTR lpsHex = strToHex;
   LPSTR lpScriptType = strScriptType[wScriptType];

   // 清空垃圾内存，以空数据填充	
   memset(strScrpitEntryLast, 0, sizeof(strScrpitEntryLast));
   memset(strScrpitEntry, 0, sizeof(strScrpitEntry));
   memset(strToHex, 0, sizeof(strToHex));
   memset(strScriptMessage, 0, sizeof(strScriptMessage));

   do
   {
      // 非该函数本身递归则重置跳转次数及初始脚本地址
      if (!wRoundOfJump)
      {
         wStartScriptAddress = wScrpitEntry;
         wJumpRound = 0;
      }

      // 拼接无作用地址 Address: 0x0000
      sprintf(strScrpitEntry, "%s\n", strZeroBug);

      while (TRUE)
      {
         switch (pScript->wOperation)
         {
         case 0x0000:
            fEnded = TRUE;
            break;

         case 0x0001:
            if (wRoundOfJump)
               fEnded = TRUE;
            break;

         case 0x0002:
            // 参数二为 0，此指令的跳转次数为永久，不再读取后面的脚本
            // 停止读取，若是调用参数二指向的地址，若遇到其他跳转语句，则直接停止脚本读取，不会返回继续
            // 若是跳转过来读取到的该指令，将不会进行二次跳转，不再读取后面的脚本
            if (wRoundOfJump)
            {
               fEnded = TRUE;
            }

         case 0x0003:
            if (pScript->rgwOperand[1] == 0)
            {
               //
               // 若参数二不为零则另外去记录
               //
               fEnded = TRUE;
            }
            break;

         default:
            break;

         }

         // 拼接地址上的别名
         sprintf(strScrpitEntry, "%s@%s:", strScrpitEntry, UTIL_DecToHex(i++, lpsHex, 4));

         // 拼接指令头
         sprintf(strScrpitEntry, "%s %s", strScrpitEntry, UTIL_DecToHex(pScript->wOperation, lpsHex, 4));

         // 取得指令号
         wOperation = (pScript->wOperation == 0xFFFF) ? (wScriptMessageLen - 1) : pScript->wOperation;

         // 拼接参数体
         for (wIndex = 0; wIndex < sizeof(pScript->rgwOperand) / sizeof(pScript->rgwOperand[0]); wIndex++)
         {
            // 以下情况分开处理
            switch (strSrciptMessages[wOperation].szSrcMessOperand[wIndex])
            {
            case kScriptID:
               if (pScript->rgwOperand[wIndex] == 0x0000)
                  UTIL_DecToHex(0xFFFF, lpsHex, 4);

               iJumpLen = pScript->rgwOperand[wIndex] - wStartScriptAddress;

               if (iJumpLen >= 0 && iJumpLen < PAL_GetScriptLen(wStartScriptAddress, wRoundOfJump))
                  UTIL_DecToHex(iJumpLen, lpsHex, 4);
               else
                  UTIL_DecToHex(0xF000 + wJumpRound, lpsHex, 4);

               break;

            default:
               UTIL_DecToHex(pScript->rgwOperand[wIndex], lpsHex, 4);
               break;

            }

            sprintf(strScrpitEntry, "%s %s", strScrpitEntry, lpsHex);
         }

         // 拼接注释
         sprintf(strScrpitEntry, "%s %s", strScrpitEntry, PAL_GetScriptMessage(pScript, strScriptMessage));

         // 释放内存
         memset(strScriptMessage, 0, sizeof(strScriptMessage));

         // 该条指令拼接完毕，换行
         sprintf(strScrpitEntry, "%s\n", strScrpitEntry);

         // 开启递归，并备份先前整理的数据
         if (fScriptJump && wThisJumpRound != wJumpRound)
         {
            // 同步递归备份开关
            wThisJumpRound = wJumpRound;

            // 备份脚本地址
            wScriptAddressLast = wScriptAddress;

            // 设置递归跳转脚本地址
            wScriptAddress = pScript->rgwOperand[wIndex];

            // 备份递归前整合的文本
            sprintf(strScrpitEntry, "%s", strScrpitEntry);

            // 清理当前整合文本的内存--
            memset(strScrpitEntry, 0, sizeof(strScrpitEntry));
         }

         // 若读取到停止执行命令则跳出
         if (fEnded)
         {
            if (fScriptJump)
            {
               // 关闭递归
               fScriptJump = FALSE;

               // 保存递归整合文档
               UTIL_SaveTextFile(strScriptSavePath, "w", strScrpitEntry, "Script%s_%03d.TSV", lpScriptType, wJumpRound);

               // 恢复递归前的数据

            }

            break;
         }

         // 迭代
         pScript = &(*p).lprgScriptEntry[++wScriptAddress];
         wOperation = &pScript->wOperation;

      }

   } while (TRUE);

   // 保存 进场脚本文件 文件 ScriptIO.txt
   UTIL_SaveTextFile(strScriptSavePath, "w", strScrpitEntry, "Script%s_%03d.TSV", lpScriptType, wRoundOfJump);

   /*++
   // 创建 进场脚本文件 文件 ScriptIO.txt
   if ((fpScript = UTIL_OpenFileAtPathForMode(strScriptSavePath, PAL_va(1, "Script%s_%03d.TSV", lpScriptType, wRoundOfJump), "w")) == NULL)
   {
      return;
   }

   // 将地址上的脚本写入文件 ScriptIO.txt
   fputs(strScrpitEntry, fpScript);

   // 关闭文件 场景总览  SCENE.TXT
   fclose(fpScript);
   --*/
}
#endif // DEBUG_DO_WHILE_PAL_SaveGameScript


VOID
PAL_SaveGameScene(
   VOID
)
{
   WORD          wSceneIndex, wEventIndex, wThisEventIndex, wThisSceneEventNum, nScene;

   SCENE* lpScene;
   SCENE         rgThisScene;
   SCENE         rgNextScene;

   LPEVENTOBJECT lpEvent;
   EVENTOBJECT   lpThisEvent;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextEvent = (LPSTR)UTIL_malloc(0xFFFF * 256);

   FILE* fpTextFile;

   // 创建文本文件
   if ((fpTextFile = UTIL_OpenFileAtPathForMode(lpszPalMassagesPath, "Event.TSV", "w")) == NULL)
   {
      return;
   }

   wSceneIndex = 0;
   wEventIndex = 0;
   wThisEventIndex = 0;
   wThisSceneEventNum = 0;

   lpScene = &gpGlobals->g.rgScene;
   lpEvent = &gpGlobals->g.lprgEventObject;
   lpThisEvent = lpEvent[wEventIndex];

   lpsHex = (LPSTR)UTIL_malloc(5);
   lpTextEvent = (LPSTR)UTIL_malloc(0xFFFF * 256);

   // 初始化内存，用空数据填充
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextEvent, 0, sizeof(lpTextEvent));

   nScene = sizeof(*lpScene);

   LPCSTR lpEventTH = "事件号\t隐时间\t方位X\t方位Y\t起始层\t触发脚本\t自动脚本\t状态\t触发模式\t形象号\t形象数\t方向\t当前帧数\t空闲帧数\t无效\t总帧数\t空闲计数\n";

   // 拼接总表头
   fprintf(fpTextFile, lpEventTH);

   // 拼接全部数据
   for (; wSceneIndex < nScene - 1; wSceneIndex++)
   {
      // 获取当前场景数据
      rgThisScene = lpScene[wSceneIndex];

      // 获取下一个场景数据
      rgNextScene = lpScene[wSceneIndex + 1];

      // 获取当前场景事件数量
      wThisSceneEventNum = rgNextScene.wEventObjectIndex - rgThisScene.wEventObjectIndex;

      // 拼接场景名称
      sprintf(lpTextEvent, "%s\n场景名称：“%s”\n地图名称：“%s”\n", lpTextEvent, SceneID[wSceneIndex + ((gpGlobals->fIsWIN95) ? 0 : 1)][1], MapID[rgThisScene.wMapNum][1]);

      // 拼接表头
      strcat(lpTextEvent, "场景号\t地图号\t进入脚本\t传送脚本\t原始事件数\n");

      // 拼接场景参数
      sprintf(lpTextEvent, "%s#%03d\t%03d\t%s\t", lpTextEvent, wSceneIndex + 1, rgThisScene.wMapNum, UTIL_DecToHex(rgThisScene.wScriptOnEnter, lpsHex, 4));

      sprintf(lpTextEvent, "%s%s\t%d\n", lpTextEvent, UTIL_DecToHex(rgThisScene.wScriptOnTeleport, lpsHex, 4), wThisSceneEventNum);

      // 拼接事件表头
      strcat(lpTextEvent, lpEventTH);

      // 拼接事件数据
      for (; wEventIndex < rgNextScene.wEventObjectIndex; wEventIndex++)
      {
         // 显示解档进度.....
         printf("Scene: %d / %d, Event: %d / %d\n", wSceneIndex, MAX_SCENES, wEventIndex + 1, gpGlobals->g.nEventObject);

         // 当前事件数据
         lpThisEvent = gpGlobals->g.lprgEventObject[wEventIndex];

         // 拼接事件名称
         sprintf(lpTextEvent, "%s事件名称：“%s”\n", lpTextEvent, SpriteID[lpThisEvent.wSpriteNum][1]);

         // 拼接事件参数
         sprintf(lpTextEvent, "%s@%s\t%d\t%d\t%d\t%d\t", lpTextEvent, UTIL_DecToHex(wEventIndex + 1, lpsHex, 4), (SHORT)lpThisEvent.sVanishTime, lpThisEvent.x, lpThisEvent.y, (SHORT)lpThisEvent.sLayer);

         strcat(lpTextEvent, UTIL_DecToHex(lpThisEvent.wTriggerScript, lpsHex, 4));

         sprintf(lpTextEvent, "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", lpTextEvent, UTIL_DecToHex(lpThisEvent.wAutoScript, lpsHex, 4),
            (SHORT)lpThisEvent.sState, lpThisEvent.wTriggerMode, lpThisEvent.wSpriteNum, lpThisEvent.nSpriteFrames, lpThisEvent.wDirection, lpThisEvent.wCurrentFrameNum,
            lpThisEvent.nScriptIdleFrame, lpThisEvent.wSpritePtrOffset, lpThisEvent.nSpriteFramesAuto, lpThisEvent.wScriptIdleFrameCountAuto);

         // 保存 进场脚本文件 文件 ScriptEntry.txt
         fprintf(fpTextFile, lpTextEvent);

         // 释放内存
         memset(lpTextEvent, 0, sizeof(lpTextEvent));

         // 解档进度.....
         printf("Scene: %d / %d, Event: %d / %d\n", wEventIndex + 1, rgNextScene.wEventObjectIndex, wSceneIndex + 1, nScene);
      }
   }

   // 释放内存
   free(lpsHex);
   free(lpTextEvent);

   UTIL_CloseFile(fpTextFile);
}

VOID
PAL_SaveGameObject(
   VOID
)
{
   INT     i;
   WORD    wObjectIndex, wObjectType, wMakeAnything, wObjectArgsIndex, wThisWord;
   WORD    wBaseIndex, wBaseArgsIndex, wBaseArgsLen;
   LPSTR   lpsHex;
   LPSTR   lpTextObect;
   LPSTR   lpThisWord;
   OBJECT *lpObject;
   OBJECT  rgThisObject;
   LPMAGIC lpThisMagic;
   LPENEMY lpThisEnemy;
   LPCSTR  lpTextNames[] = {
      "ObjLabel.TSV", "ObjPlayer.TSV", "ObjItem.TSV", "ObjMagic.TSV",
      "ObjEnemy.TSV", "ObjPoison.TSV", "ObjOther.TSV"
   };
   LPCSTR  lpObjectTableHead[][3] = {
      {
         "标签编号\t系统名称\t无效数据\t无效数据\t无效数据\t无效数据\t无效数据\t",
         "无效数据\t",
         "无效数据"
      },
      {
         "队员编号\t队员名称\t无效数据\t无效数据\t愤怒脚本\t虚弱数据\t无效数据\t",
         "无效数据\t",
         "无效数据"
      },
      {
         "道具编号\t道具名称\t图像编号\t售价\t使用脚本\t装备脚本\t投掷脚本\t",
         "描述脚本\t",
         "可使用\t可装备\t可投掷\t食掷减少\t作用全体\t可典当\t可装备者"
      },
      {
         "仙术编号\t仙术名称\t设定编号\t图像编号\t攻击模式\tX轴偏移\tY轴偏移\t图层偏移或神形\t特效速度\t图像残留\t音效延迟\t总耗时\t场景震动\t场景波动\t无效数据\t消耗真气\t基础伤害\t仙术系属\t音效编号\t无效数据\t后序脚本\t前序脚本\t无效数据\t",
         "描述脚本\t",
         "战前可用\t战时可用\t无效数据\t作用敌方\t作用全体"
      },
      {
         "敌人编号\t敌人名称\t设定编号\t蠕动帧数\t施法帧数\t进攻帧数\t蠕动速度\t每帧延迟\tY轴偏移\t攻前音效\t击中音效\t施法音效\t死亡音效\t攻击音效\t体力\t可得经验\t可得金钱\t修行\t仙术描述\t默认仙术\t施法概率\t附带描述\t攻击附带\t附带概率\t偷窃描述\t偷窃可得\t可偷数量\t武术\t灵力\t防御\t身法\t吉运\t毒抗\t风抗\t雷抗\t水抗\t火抗\t土抗\t物抗\t行动次数\t灵葫值\t巫抗\t战始脚本\t胜利脚本\t战斗脚本\t",
         "无效数据\t",
         "无效数据"
      },
      {
         "毒性编号\t毒性名称\t级别\t颜色\t我中脚本\t无效数据\t敌中脚本\t",
         "无效数据\t",
         "无效数据"
      },
      {
         "对象编号\t对象名称\t关联参数\t属性参数\t脚本1\t脚本2\t脚本3\t",
         "新增参数\t",
         "作用参数"
      }
   };

   FILE* fpTextFile[sizeof(lpTextNames) / sizeof(lpTextNames[0])];

   // 创建文本文件
   for (i = 0; i < sizeof(lpTextNames) / sizeof(lpTextNames[0]); i++)
   {
      if ((fpTextFile[i] = UTIL_OpenFileAtPathForMode(lpszPalMassagesPath, lpTextNames[i], "w")) == NULL)
      {
         return;
      }

      // 写入表头
      fprintf(fpTextFile[i], lpObjectTableHead[i][0]);
      if (gpGlobals->fIsWIN95) fprintf(fpTextFile[i], lpObjectTableHead[i][1]);
      fprintf(fpTextFile[i], lpObjectTableHead[i][2]);
   }

   wObjectIndex = 0;
   wObjectType = 0;

   lpObject = &gpGlobals->g.rgObject;
   rgThisObject = lpObject[wObjectIndex];

   lpsHex = (LPSTR)UTIL_malloc(5);
   lpTextObect = (LPSTR)UTIL_malloc(0xFFFF * 256);

   for (wObjectIndex = 0; wObjectIndex < g_TextLib.nWords; wObjectIndex++)
   {
      // 清空垃圾内存，以空数据填充
      memset(lpTextObect, 0, sizeof(lpTextObect));
      memset(lpsHex, 0, sizeof(lpsHex));

      // 获取当前对象
      rgThisObject = lpObject[wObjectIndex];

      // 获取当前对象名称
      lpThisWord = PAL_GetWord(wObjectIndex);

      if (!strcmp(lpThisWord, "")) lpThisWord = "<NULL>";

      // 拼接 对象ID 及名称
      sprintf(lpTextObect, "%s\n@%s\t%s", lpTextObect, UTIL_DecToHex(wObjectIndex, lpsHex, 4), lpThisWord);

      // 检测对象类型（注意：仅原版可放心食用......）
      if (wObjectIndex >= 0x0000 && wObjectIndex <= 0x0023 && wObjectIndex != 0x0018
         || wObjectIndex >= 0x002A && wObjectIndex <= 0x003C)
      {
         // 系统文本标签
         wObjectType = 0;

         // 拼接脚本....
         for (wObjectArgsIndex = 0; wObjectArgsIndex < (6 + gpGlobals->fIsWIN95); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }
      }
      else if (wObjectIndex >= 0x0024 && wObjectIndex <= 0x0029)
      {
         // 队员对象
         wObjectType = 1;

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d\t%d", lpTextObect, rgThisObject.rgwData[0], rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < (6 + (gpGlobals->fIsWIN95)); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }
      }
      else if (wObjectIndex >= 0x003D && wObjectIndex <= 0x0126)
      {
         // 道具对象
         wObjectType = 2;

         wBaseArgsLen = sizeof(MAGIC) / sizeof(WORD);

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d\t%d\t", lpTextObect, rgThisObject.rgwData[0], rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < ((gpGlobals->fIsWIN95) ? 6 : 5); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s%s\t", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }

         // 解档道具掩码
         for (wMakeAnything = 0; wMakeAnything < ((wObjectType == 3) ? 5 : ((wObjectType == 2) ? 6 : 0)); wMakeAnything++)
         {
            sprintf(lpTextObect, "%s%d\t", lpTextObect, !!(rgThisObject.rgwData[wObjectArgsIndex] & (1 << wMakeAnything)));
         }

         for (wMakeAnything = 0; wMakeAnything < MAX_PLAYER_ROLES; wMakeAnything++)
         {
            // 写两个 !! 是指取两次真假，将非 0 值化为 1
            sprintf(lpTextObect, "%s%d", lpTextObect, !!(rgThisObject.rgwData[wObjectArgsIndex] & (kItemFlagEquipableByPlayerRole_First << wMakeAnything)));
         }
      }
      else if (wObjectIndex == 0x0018 || wObjectIndex >= 0x0127 && wObjectIndex <= 0x018D)
      {
         // 仙术对象
         wObjectType = 3;

         // 获取结构体成员数
         wBaseArgsLen = sizeof(MAGIC) / sizeof(WORD);

         // 获取仙术类型
         wBaseIndex = rgThisObject.rgwData[0];

         // 获取当前仙术基数据
         lpThisMagic = &gpGlobals->g.lprgMagic[wBaseIndex];

         // 拼接仙术基索引
         sprintf(lpTextObect, "%s\t%d", lpTextObect, wBaseIndex);

         // 拼接仙术基参数
         for (wBaseArgsIndex = 0; wBaseArgsIndex < wBaseArgsLen; wBaseArgsIndex++)
         {
            switch (wBaseArgsIndex)
            {
            case 0:
            case 2:
            case 3:
            case 5:
            case 6:
            case 8:
            case 13:
            case 15:
               sprintf(lpTextObect, "%s\t%d", lpTextObect, ((SHORT*)lpThisMagic)[wBaseArgsIndex]);
               break;

            case 4:
               if (lpThisMagic->wType == kMagicTypeSummon)
                  sprintf(lpTextObect, "%s\t%d", lpTextObect, lpThisMagic->rgSpecific.wSummonEffect);
               else
                  sprintf(lpTextObect, "%s\t%d", lpTextObect, lpThisMagic->rgSpecific.sLayerOffset);
               break;

            default:
               sprintf(lpTextObect, "%s\t%d", lpTextObect, ((WORD*)lpThisMagic)[wBaseArgsIndex]);
               break;
            }
         }

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d", lpTextObect, rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < (5 + (gpGlobals->fIsWIN95)); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }

         // 解档仙术掩码
         for (wMakeAnything = 0; wMakeAnything < ((wObjectType == 3) ? 5 : ((wObjectType == 2) ? 6 : 0)); wMakeAnything++)
         {
            sprintf(lpTextObect, "%s\t%d", lpTextObect, !!(rgThisObject.rgwData[wObjectArgsIndex] & (1 << wMakeAnything)));
         }
      }
      else if (wObjectIndex >= 0x018E && wObjectIndex <= 0x0226)
      {
         // 敌人对象
         wObjectType = 4;

         // 获取结构体成员数
         wBaseArgsLen = sizeof(ENEMY) / sizeof(WORD);

         // 获取敌方类型
         wBaseIndex = rgThisObject.rgwData[0];

         // 获取当前敌人基数据
         lpThisEnemy = &gpGlobals->g.lprgEnemy[wBaseIndex];

         // 拼接敌人基索引
         sprintf(lpTextObect, "%s\t%d", lpTextObect, wBaseIndex);

         // 拼接敌人基参数
         for (wBaseArgsIndex = 0; wBaseArgsIndex < wBaseArgsLen; wBaseArgsIndex++)
         {
            // 翻译对象名称
            if (wBaseArgsIndex == 15 || wBaseArgsIndex == 17 || wBaseArgsIndex == 19)
            {
               // 获取当前对象名称
               lpThisWord = PAL_GetWord(((WORD*)lpThisEnemy)[wBaseArgsIndex]);

               if (!strcmp(lpThisWord, "")) lpThisWord = "<NULL>";

               // 拼接 对象ID 及名称
               sprintf(lpTextObect, "%s\t%s", lpTextObect, lpThisWord);
            }

            switch (wBaseArgsIndex)
            {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
               sprintf(lpTextObect, "%s\t%d", lpTextObect, ((SHORT*)lpThisEnemy)[wBaseArgsIndex]);
               break;

            default:
               sprintf(lpTextObect, "%s\t%d", lpTextObect, ((WORD*)lpThisEnemy)[wBaseArgsIndex]);
               break;
            }
         }

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d", lpTextObect, rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < (6 + (gpGlobals->fIsWIN95)); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }
      }
      else if (wObjectIndex >= 0x0227 && wObjectIndex <= 0x0234)
      {
         // 毒性对象
         wObjectType = 5;

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d\t%d", lpTextObect, rgThisObject.rgwData[0], rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < (6 + (gpGlobals->fIsWIN95)); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }
      }
      else
      {
         // 用户自定义的对象
         wObjectType = 6;

         // 拼接对象参数
         sprintf(lpTextObect, "%s\t%d\t%d", lpTextObect, rgThisObject.rgwData[0], rgThisObject.rgwData[1]);

         // 拼接脚本....
         for (wObjectArgsIndex = 2; wObjectArgsIndex < (6 + (gpGlobals->fIsWIN95)); wObjectArgsIndex++)
         {
            sprintf(lpTextObect, "%s\t%s", lpTextObect, UTIL_DecToHex(rgThisObject.rgwData[wObjectArgsIndex], lpsHex, 4));
         }
      }

      // 逐行写入 进场对象文件
      fprintf(fpTextFile[wObjectType], lpTextObect);

      // 解档进度.....
      printf("Object: %d / %d\n", wObjectIndex + 1, g_TextLib.nWords);
   }

   // 将仙术说明写入文档。。。
   fprintf(fpTextFile[3], "\n\n\n提示：请注意攻击模式！\t\t\t提示：请注意仙术系属！\n0 = 单特效（攻击敌方单人）\t\t\t0 = 剑系（无系）\n1 = 多特效（攻击敌方全体）\t\t\t1 -> 5 = 五灵\n2 = 单特效（攻击敌方全体）\t\t\t6 = 毒系\n3 = 全屏单特效（攻击敌方全体）\t\t\t7 = 防守系（治愈系，非）\n4 = 单特效（我方单人防守）\n5 = 多特效（我方全体防守）\n8 = 施法者自己觉醒（施法者变身时的过渡效果）\n9 = 召唤神\n\n\n");

   // 释放内存
   free(lpsHex);
   free(lpTextObect);

   for (i = 0; i < sizeof(lpTextNames) / sizeof(lpTextNames[0]); i++) UTIL_CloseFile(fpTextFile[i]);
}

VOID
PAL_SaveGameScript(
   VOID
)
{
   LPSCRIPTENTRY pScript;
   LPSTR         lpsHex;
   LPSTR         lpScriptMessage;
   LPSTR         lpTextScrpitEntry;
   WORD          wEntryIndex, wOperandIndex, wOperandLen;

   FILE* fpTextFile;

   // 创建文本文件
   if ((fpTextFile = UTIL_OpenFileAtPathForMode(lpszPalMassagesPath, PAL_va(1, "ScriptEntry.TSV"), "w")) == NULL)
   {
      return;
   }

   lpsHex = (LPSTR)UTIL_malloc(5);
   lpScriptMessage = (LPSTR)UTIL_malloc(256);
   lpTextScrpitEntry = (LPSTR)UTIL_malloc(0xFFFF * 256);

   wOperandLen = sizeof(pScript->rgwOperand) / sizeof(pScript->rgwOperand[0]);

   // 清空垃圾内存，以空数据填充	
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpScriptMessage, 0, sizeof(lpScriptMessage));
   memset(lpTextScrpitEntry, 0, sizeof(lpTextScrpitEntry));

   for (wEntryIndex = 0; wEntryIndex < gpGlobals->g.nScriptEntry; wEntryIndex++)
   {
      // 获取当前地址的脚本条目
      pScript = &gpGlobals->g.lprgScriptEntry[wEntryIndex];

      // 拼接地址上的别名
      sprintf(lpTextScrpitEntry, "%s@%s:", lpTextScrpitEntry, UTIL_DecToHex(wEntryIndex, lpsHex, 4));

      // 拼接指令头
      sprintf(lpTextScrpitEntry, "%s %s", lpTextScrpitEntry, UTIL_DecToHex(pScript->wOperation, lpsHex, 4));

      // 拼接参数体
      for (wOperandIndex = 0; wOperandIndex < 3; wOperandIndex++)
      {
         sprintf(lpTextScrpitEntry, "%s %s", lpTextScrpitEntry, UTIL_DecToHex(pScript->rgwOperand[wOperandIndex], lpsHex, 4));
      }

      // 拼接注释并换行
      sprintf(lpTextScrpitEntry, "%s %s\n", lpTextScrpitEntry, PAL_GetScriptMessage(pScript, lpScriptMessage));

      // 保存 进场脚本文件 文件 ScriptEntry.txt
      fprintf(fpTextFile, lpTextScrpitEntry);

      // 释放内存
      memset(lpScriptMessage, 0, sizeof(lpScriptMessage));
      memset(lpTextScrpitEntry, 0, sizeof(lpTextScrpitEntry));

      printf("Script:  %d / %d\n", wEntryIndex + 1, gpGlobals->g.nScriptEntry);
   }

   // 释放内存
   free(lpsHex);
   free(lpScriptMessage);
   free(lpTextScrpitEntry);

   // 关闭文件
   UTIL_CloseFile(fpTextFile);
}

VOID
PAL_SaveGameBaseStore(
   VOID
)
{
   WORD  wStoreIndex, wThisObject, wStoreObjIndex, wThisWord;

   LPSTORE lpStore = gpGlobals->g.lprgStore;
   STORE rgThisStore;

   OBJECT* lpObject = &gpGlobals->g.rgObject;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextStore = (LPSTR)UTIL_malloc(0xFFFF * 256);
   LPSTR lpThisStoreObject = (LPSTR)UTIL_malloc(256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextStore, 0, sizeof(lpTextStore));
   memset(lpThisStoreObject, 0, sizeof(lpThisStoreObject));

   // 拼接表头
   strcat(lpTextStore, "店铺编号\t商品1\t商品2\t商品3\t商品4\t商品5\t商品6\t商品7\t商品8\t商品9\t商品详情\n");

   for (wStoreIndex = 0; wStoreIndex < gpGlobals->g.nStore; wStoreIndex++)
   {
      // 拼接索引
      sprintf(lpTextStore, "%s@%s", lpTextStore, UTIL_DecToHex(wStoreIndex, lpsHex, 4));

      // 获取当前商店
      rgThisStore = lpStore[wStoreIndex];

      // 拼接商品对象编号
      for (wStoreObjIndex = 0; wStoreObjIndex < MAX_STORE_ITEM; wStoreObjIndex++)
      {
         wThisWord = rgThisStore.rgwItems[wStoreObjIndex];

         sprintf(lpTextStore, "%s\t%s", lpTextStore, UTIL_DecToHex(wThisWord, lpsHex, 4));
         strcat(lpThisStoreObject, (wThisWord == 0x0000) ? lpZeroWord : PAL_GetWord(wThisWord));
         strcat(lpThisStoreObject, "|");
      }

      sprintf(lpTextStore, "%s\t<|%s>\n", lpTextStore, lpThisStoreObject);
      memset(lpThisStoreObject, 0, sizeof(lpThisStoreObject));

      // 解档进度.....
      printf("Store: %d / %d\n", wStoreIndex + 1, gpGlobals->g.nStore);

   }

   // 保存文件 Store.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextStore, "Store.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextStore);
}

VOID
PAL_SaveGameBaseEnemy(
   VOID
)
{
   WORD wEnemyBaseIndex, wEnemyBaseArgsIndex, wThisWord, wEnemyBaseArgsLen = sizeof(ENEMY) / sizeof(WORD);
   WORD* lpThisEnemy;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextEnemyBase = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextEnemyBase, 0, sizeof(lpTextEnemyBase));

   strcat(lpTextEnemyBase, "敌人编号\t蠕动帧数\t施法帧数\t进攻帧数\t蠕动速度\t每帧延迟\tY轴偏移\t攻前音效\t击中音效\t施法音效\t死亡音效\t攻击音效\t体力\t可得经验\t可得金钱\t修行\t仙术描述\t默认仙术\t施法概率\t附带描述\t攻击附带\t附带概率\t偷窃描述\t偷窃可得\t可偷数量\t武术\t灵力\t防御\t身法\t吉运\t毒抗\t风抗\t雷抗\t水抗\t火抗\t土抗\t物抗\t行动次数\t灵葫值\n");

   for (wEnemyBaseIndex = 0; wEnemyBaseIndex < gpGlobals->g.nEnemy; wEnemyBaseIndex++)
   {
      // 获取当前敌人基数据
      lpThisEnemy = (WORD*)&gpGlobals->g.lprgEnemy[wEnemyBaseIndex];

      // 拼接敌人基索引
      sprintf(lpTextEnemyBase, "%s@%s", lpTextEnemyBase, UTIL_DecToHex(wEnemyBaseIndex, lpsHex, 4));

      // 拼接敌人基参数
      for (wEnemyBaseArgsIndex = 0; wEnemyBaseArgsIndex < wEnemyBaseArgsLen; wEnemyBaseArgsIndex++)
      {
         if (wEnemyBaseArgsIndex == 15 || wEnemyBaseArgsIndex == 17 || wEnemyBaseArgsIndex == 19)
         {
            //
            // 默认仙术
            //
            // 获取当前仙术对象编号
            wThisWord = lpThisEnemy[wEnemyBaseArgsIndex];

            sprintf(lpTextEnemyBase, "%s\t%s\t%s", lpTextEnemyBase, (wThisWord == 0x0000) ? lpZeroWord : PAL_GetWord(wThisWord), UTIL_DecToHex(wThisWord, lpsHex, 4));
         }
         else if (wEnemyBaseArgsIndex >= 6 && wEnemyBaseArgsIndex <= 10)
            sprintf(lpTextEnemyBase, "%s\t%d", lpTextEnemyBase, (SHORT)lpThisEnemy[wEnemyBaseArgsIndex]);
         else
            sprintf(lpTextEnemyBase, "%s\t%d", lpTextEnemyBase, lpThisEnemy[wEnemyBaseArgsIndex]);
      }

      // 换行
      strcat(lpTextEnemyBase, "\n");

      // 解档进度.....
      printf("EnemyBase: %d / %d\n", wEnemyBaseIndex + 1, gpGlobals->g.nEnemy);

   }

   // 保存文件 EnemyBase.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextEnemyBase, "EnemyBase.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextEnemyBase);
}

VOID
PAL_SaveGameBaseEnemyTeam(
   VOID
)
{
   WORD  wEnemyTeamIndex, wThisEnemyTeam, wStoreObjIndex, wThisWord;

   LPENEMYTEAM lpEnemyTeam = gpGlobals->g.lprgEnemyTeam;
   ENEMYTEAM rgThisEnemyTeam;

   OBJECT* lpObject = &gpGlobals->g.rgObject;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextEnemyTeam = (LPSTR)UTIL_malloc(0xFFFF * 256);
   LPSTR lpThisEnemyTeam = (LPSTR)UTIL_malloc(256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextEnemyTeam, 0, sizeof(lpTextEnemyTeam));
   memset(lpThisEnemyTeam, 0, sizeof(lpThisEnemyTeam));

   // 拼接表头
   strcat(lpTextEnemyTeam, "队列编号\t目标号1\t目标号2\t目标号3\t目标号4\t目标号5\t队列详情\n");

   for (wEnemyTeamIndex = 0; wEnemyTeamIndex < gpGlobals->g.nEnemyTeam; wEnemyTeamIndex++)
   {
      // 拼接索引
      sprintf(lpTextEnemyTeam, "%s@%s", lpTextEnemyTeam, UTIL_DecToHex(wEnemyTeamIndex, lpsHex, 4));

      // 获取当前商店
      rgThisEnemyTeam = lpEnemyTeam[wEnemyTeamIndex];

      // 拼接敌人对象编号
      for (wStoreObjIndex = 0; wStoreObjIndex < MAX_ENEMIES_IN_TEAM; wStoreObjIndex++)
      {
         wThisWord = rgThisEnemyTeam.rgwEnemy[wStoreObjIndex];

         sprintf(lpTextEnemyTeam, "%s\t%s", lpTextEnemyTeam, UTIL_DecToHex(wThisWord, lpsHex, 4));
         strcat(lpThisEnemyTeam, (wThisWord == 0xFFFF) ? lpZeroWord : ((wThisWord == 0x0000) ? lpNullWord : PAL_GetWord(wThisWord)));
         strcat(lpThisEnemyTeam, "|");
      }

      sprintf(lpTextEnemyTeam, "%s\t<|%s>\n", lpTextEnemyTeam, lpThisEnemyTeam);
      memset(lpThisEnemyTeam, 0, sizeof(lpThisEnemyTeam));

      // 解档进度.....
      printf("EnemyTeam: %d / %d\n", wEnemyTeamIndex + 1, gpGlobals->g.nEnemyTeam);

   }

   // 保存文件 EnemyTeam.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextEnemyTeam, "EnemyTeam.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextEnemyTeam);
   free(lpThisEnemyTeam);
}

VOID
PAL_SaveGameBaseMagic(
   VOID
)
{
   WORD wMagicBaseIndex, wMagicBaseArgsIndex, wMagicBaseArgsLen = sizeof(MAGIC) / sizeof(WORD);
   WORD* lpThisMagic;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextMagicBase = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextMagicBase, 0, sizeof(lpTextMagicBase));

   // 拼接提示语
   strcat(lpTextMagicBase, "提示：请注意攻击模式！\t\t\t提示：请注意仙术系属！\n0 = 单特效（攻击敌方单人）\t\t\t0 = 剑系（无系）\n1 = 多特效（攻击敌方全体）\t\t\t1 -> 5 = 五灵\n2 = 单特效（攻击敌方全体）\t\t\t6 = 毒系\n3 = 全屏单特效（攻击敌方全体）\t\t7 = 防守系（治愈系，非）\n4 = 单特效（我方单人防守）\n5 = 多特效（我方全体防守）\n8 = 施法者自己觉醒（施法者变身时的过渡效果）\n9 = 召唤神\n\n\n");

   for (wMagicBaseIndex = 0; wMagicBaseIndex < gpGlobals->g.nMagic; wMagicBaseIndex++)
   {
      if (wMagicBaseIndex % 0x0010 == 0)
      {
         // 拼接表头
         strcat(lpTextMagicBase, "\n仙术编号\t图像编号\t攻击模式\tX轴偏移\tY轴偏移\t神形编号\t特效速度\t图像残留\t音效延迟\t总耗时\t场景震动\t场景波动\t无效数据\t消耗真气\t基础伤害\t仙术系属\t音效编号\n");
      }

      // 获取当前仙术基数据
      lpThisMagic = (WORD*)&gpGlobals->g.lprgMagic[wMagicBaseIndex];

      // 拼接仙术基索引
      sprintf(lpTextMagicBase, "%s@%s", lpTextMagicBase, UTIL_DecToHex(wMagicBaseIndex, lpsHex, 4));

      // 拼接仙术基参数
      for (wMagicBaseArgsIndex = 0; wMagicBaseArgsIndex < wMagicBaseArgsLen; wMagicBaseArgsIndex++)
      {
         if (wMagicBaseArgsIndex == 5 || wMagicBaseArgsIndex == 15)
            sprintf(lpTextMagicBase, "%s\t%d", lpTextMagicBase, (SHORT)lpThisMagic[wMagicBaseArgsIndex]);
         else
            sprintf(lpTextMagicBase, "%s\t%d", lpTextMagicBase, lpThisMagic[wMagicBaseArgsIndex]);
      }

      // 换行
      strcat(lpTextMagicBase, "\n");

      // 解档进度.....
      printf("MagicBase: %d / %d\n", wMagicBaseIndex + 1, gpGlobals->g.nMagic);

   }

   // 保存文件 MagicBase.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextMagicBase, "MagicBase.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextMagicBase);
}

VOID
PAL_SaveGameBaseBattleField(
   VOID
)
{
   WORD  wBattleFieldIndex, wBattleFieldArgsIndex;

   LPBATTLEFIELD lpBattleField = gpGlobals->g.lprgBattleField;
   WORD* lpThisBattleField;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextBattleField = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextBattleField, 0, sizeof(lpTextBattleField));

   // 拼接表头
   strcat(lpTextBattleField, "战场编号\t场景波动\t风抗\t雷抗\t水抗\t火抗\t土抗\t战场环境\n");

   for (wBattleFieldIndex = 0; wBattleFieldIndex < gpGlobals->g.nBattleField; wBattleFieldIndex++)
   {
      // 获取当前战场
      lpThisBattleField = (WORD*)&lpBattleField[wBattleFieldIndex];

      // 拼接索引
      sprintf(lpTextBattleField, "%s@%s\t%d", lpTextBattleField, UTIL_DecToHex(wBattleFieldIndex, lpsHex, 4), lpThisBattleField[0]);

      // 拼接战场属性
      for (wBattleFieldArgsIndex = 1; wBattleFieldArgsIndex <= NUM_MAGIC_ELEMENTAL; wBattleFieldArgsIndex++)
      {
         sprintf(lpTextBattleField, "%s\t%d", lpTextBattleField, (SHORT)lpThisBattleField[wBattleFieldArgsIndex]);
      }

      // 拼接战场名称
      sprintf(lpTextBattleField, "%s\t%s\n", lpTextBattleField, PictureID[wBattleFieldIndex][((gpGlobals->fIsWIN95) ? 2 : 1)]);

      // 解档进度.....
      printf("BattleField: %d / %d\n", wBattleFieldIndex + 1, gpGlobals->g.nBattleField);

   }

   // 保存文件 BattleField.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextBattleField, "BattleField.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextBattleField);
}

VOID
PAL_SaveGameBaseLevelUpMagic(
   VOID
)
{
   WORD  wLevelUpMagicIndex, wLevelUpMagicArgsIndex, wThisLevel, wThisWord;

   LPLEVELUPMAGIC lpThisLevelUpMagic;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextLevelUpMagic = (LPSTR)UTIL_malloc(0xFFFF * 256);
   LPSTR lpThisMagic = (LPSTR)UTIL_malloc(256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextLevelUpMagic, 0, sizeof(lpTextLevelUpMagic));
   memset(lpThisMagic, 0, sizeof(lpThisMagic));

   // 拼接表头
   strcat(lpTextLevelUpMagic, "修行编号\t所需修行\t李逍遥仙术\t所需修行\t赵灵儿仙术\t所需修行\t林月如仙术\t所需修行\t巫后仙术\t所需修行\t阿奴仙术\t仙术详情\n");

   LPCSTR PlayerName[] = { "李", "赵", "林", "巫", "奴", "盖" };

   for (wLevelUpMagicIndex = 0; wLevelUpMagicIndex < gpGlobals->g.nLevelUpMagic; wLevelUpMagicIndex++)
   {
      // 获取当前仙术所需修行条目
      lpThisLevelUpMagic = &gpGlobals->g.lprgLevelUpMagic[wLevelUpMagicIndex];

      // 拼接索引
      sprintf(lpTextLevelUpMagic, "%s@%s", lpTextLevelUpMagic, UTIL_DecToHex(wLevelUpMagicIndex, lpsHex, 4));

      // 拼接仙术所需修行参数
      for (wLevelUpMagicArgsIndex = 0; wLevelUpMagicArgsIndex < MAX_PLAYABLE_PLAYER_ROLES; wLevelUpMagicArgsIndex++)
      {
         wThisLevel = lpThisLevelUpMagic[wLevelUpMagicArgsIndex].wLevel;
         wThisWord = lpThisLevelUpMagic[wLevelUpMagicArgsIndex].wMagic;

         sprintf(lpTextLevelUpMagic, "%s\t%d\t%s", lpTextLevelUpMagic, wThisLevel, UTIL_DecToHex(wThisWord, lpsHex, 4));

         sprintf(lpThisMagic, "%s%s-%d.%s|", lpThisMagic, PlayerName[wLevelUpMagicArgsIndex], wThisLevel, (wThisWord == 0x0000) ? lpZeroWord : PAL_GetWord(wThisWord));

      }

      // 拼接可习得的仙术
      sprintf(lpTextLevelUpMagic, "%s\t<|%s>\n", lpTextLevelUpMagic, lpThisMagic);
      memset(lpThisMagic, 0, sizeof(lpThisMagic));

      // 解档进度.....
      printf("LevelUpMagic: %d / %d\n", wLevelUpMagicIndex + 1, gpGlobals->g.nLevelUpMagic);

   }

   // 保存文件 LevelUpMagic.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextLevelUpMagic, "LevelUpMagic.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextLevelUpMagic);
   free(lpThisMagic);
}

VOID
PAL_SaveGameBaseBattleEffectIndex(
   VOID
)
{
   WORD wBattleEffectIndex;

   WORD* lpThisBattleEffectIndex;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextBattleEffectIndex = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextBattleEffectIndex, 0, sizeof(lpTextBattleEffectIndex));

   // 拼接表头
   strcat(lpTextBattleEffectIndex, "战效编号\t索引1\t索引2\t备注\n");

   for (wBattleEffectIndex = 0; wBattleEffectIndex < 10; wBattleEffectIndex++)
   {
      // 获取当前仙术所需修行条目
      lpThisBattleEffectIndex = &gpGlobals->g.rgwBattleEffectIndex[wBattleEffectIndex];

      // 拼接索引、仙术所需修行参数、备注
      sprintf(lpTextBattleEffectIndex, "%s@%s\t%d\t%d\t%s\n", lpTextBattleEffectIndex,
         UTIL_DecToHex(wBattleEffectIndex, lpsHex, 4), lpThisBattleEffectIndex[0], lpThisBattleEffectIndex[1], BattleSpriteID[wBattleEffectIndex][1]);

      // 解档进度.....
      printf("BattleEffectIndex: %d / %d\n", wBattleEffectIndex + 1, 10);

   }

   // 保存文件 BattleEffectIndex.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextBattleEffectIndex, "BattleEffectIndex.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextBattleEffectIndex);
}

VOID
PAL_SaveGameBaseEnemyPos(
   VOID
)
{
   WORD wEnemyPosIndex, wEnemyPosArgsIndex;

   LPPALPOS lpThisEnemyPos;

   PALPOS thisEnemyPos;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextEnemyPos = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextEnemyPos, 0, sizeof(lpTextEnemyPos));

   // 拼接表头
   strcat(lpTextEnemyPos, "EMPos\tPos0_X\tPos0_Y\tPos1_X\tPos1_Y\tPos2_X\tPos2_Y\tPos3_X\tPos3_Y\tPos4_X\tPos4_Y");

   for (wEnemyPosIndex = 0; wEnemyPosIndex < MAX_ENEMIES_IN_TEAM; wEnemyPosIndex++)
   {
      // 获取当前仙术所需修行条目
      lpThisEnemyPos = &gpGlobals->g.EnemyPos.pos[wEnemyPosIndex];

      // 拼接索引
      sprintf(lpTextEnemyPos, "%s\n@%s", lpTextEnemyPos, UTIL_DecToHex(wEnemyPosIndex, lpsHex, 4));

      // 拼接仙术所需修行参数
      for (wEnemyPosArgsIndex = 0; wEnemyPosArgsIndex < MAX_ENEMIES_IN_TEAM; wEnemyPosArgsIndex++)
      {
         thisEnemyPos = lpThisEnemyPos[wEnemyPosArgsIndex];
         sprintf(lpTextEnemyPos, "%s\t%d\t%d", lpTextEnemyPos, thisEnemyPos.x, thisEnemyPos.y);
      }

      // 解档进度.....
      printf("EnemyPos: %d / %d\n", wEnemyPosIndex + 1, MAX_ENEMIES_IN_TEAM);

   }

   // 保存文件 EnemyPos.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextEnemyPos, "EnemyPos.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextEnemyPos);
}

VOID
PAL_SaveGameBaseLevelUpExp(
   VOID
)
{
   WORD wLevelUpExpIndex;

   LPSTR lpsHex = (LPSTR)UTIL_malloc(5);
   LPSTR lpTextLevelUpExp = (LPSTR)UTIL_malloc(0xFFFF * 256);
   memset(lpsHex, 0, sizeof(lpsHex));
   memset(lpTextLevelUpExp, 0, sizeof(lpTextLevelUpExp));

   // 拼接表头
   strcat(lpTextLevelUpExp, "修行编号\t所需经历\t修行数\n");

   for (wLevelUpExpIndex = 0; wLevelUpExpIndex < MAX_LEVELS + 1; wLevelUpExpIndex++)
   {
      // 拼接索引
      sprintf(lpTextLevelUpExp, "%s@%s", lpTextLevelUpExp, UTIL_DecToHex(wLevelUpExpIndex, lpsHex, 4));

      // 拼接仙术所需修行参数
      sprintf(lpTextLevelUpExp, "%s\t%d\t修行%d\n", lpTextLevelUpExp, gpGlobals->g.rgLevelUpExp[wLevelUpExpIndex], wLevelUpExpIndex);

      // 解档进度.....
      printf("LevelUpExp: %d / %d\n", wLevelUpExpIndex + 1, MAX_LEVELS + 1);

   }

   // 保存文件 LevelUpExp.txt
   UTIL_SaveText(lpszPalMassagesPath, "w", lpTextLevelUpExp, "LevelUpExp.TSV");

   // 释放内存
   free(lpsHex);
   free(lpTextLevelUpExp);
}

INT
PAL_InitGlobals(
   VOID
)
/*++
  Purpose:

   Initialize global data.

  Parameters:

   None.

  Return value:

   0 = success, -1 = error.

--*/
{
   //
   // Open files
   //
   gpGlobals->f.fpFBP = UTIL_OpenRequiredFile("fbp.mkf");
   gpGlobals->f.fpMGO = UTIL_OpenRequiredFile("mgo.mkf");
   gpGlobals->f.fpBALL = UTIL_OpenRequiredFile("ball.mkf");
   gpGlobals->f.fpDATA = UTIL_OpenRequiredFile("data.mkf");
   gpGlobals->f.fpF = UTIL_OpenRequiredFile("f.mkf");
   gpGlobals->f.fpFIRE = UTIL_OpenRequiredFile("fire.mkf");
   gpGlobals->f.fpRGM = UTIL_OpenRequiredFile("rgm.mkf");
   gpGlobals->f.fpSSS = UTIL_OpenRequiredFile("sss.mkf");

   //
   // Retrieve game resource version
   //
   if (!PAL_IsWINVersion()) return -1;

   return 0;
}

VOID
PAL_FreeGlobals(
   VOID
)
/*++
  Purpose:

   Free global data.

  Parameters:

   None.

  Return value:

   None.

--*/
{
   //
   // Close all opened files
   // 关闭所有文件
   UTIL_CloseFile(gpGlobals->f.fpFBP);
   UTIL_CloseFile(gpGlobals->f.fpMGO);
   UTIL_CloseFile(gpGlobals->f.fpBALL);
   UTIL_CloseFile(gpGlobals->f.fpDATA);
   UTIL_CloseFile(gpGlobals->f.fpF);
   UTIL_CloseFile(gpGlobals->f.fpFIRE);
   UTIL_CloseFile(gpGlobals->f.fpRGM);
   UTIL_CloseFile(gpGlobals->f.fpSSS);

   //
   // Free the game data
   // 释放游戏数据
   free(gpGlobals->g.lprgEventObject);
   free(gpGlobals->g.lprgScriptEntry);
   free(gpGlobals->g.lprgStore);
   free(gpGlobals->g.lprgEnemy);
   free(gpGlobals->g.lprgEnemyTeam);
   free(gpGlobals->g.lprgMagic);
   free(gpGlobals->g.lprgBattleField);
   free(gpGlobals->g.lprgLevelUpMagic);

   // 释放对话和对象名称内存
   PAL_FreeText();

   //
   // Clear the instance
   // 清理内存
   memset(gpGlobals, 0, sizeof(GLOBALVARS));
}
