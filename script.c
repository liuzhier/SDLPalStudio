#include "script.h"

const static LPSTR strElemental[sizeof(struct tagBATTLEFIELD) / sizeof(WORD)] = { "场景波动等级", "风", "雷", "水", "火", "土" };

LPSTR
PAL_GetScriptMessage(
	LPSCRIPTENTRY lprgScriptEntry,
	LPSTR strBuffer
)
{
	GAMEDATA* pG = &gpGlobals->g;

	LPSCRIPTENTRY pScript = lprgScriptEntry;

	WORD wI, wJ, wNum, wThisWord, wOperation = pScript->wOperation, rgwOperand;

	wOperation = (wOperation == 0xFFFF) ? (wScriptMessageLen - 1) : wOperation;

	CHAR strThisWord[11];
	CHAR strToHex[5];
	CHAR strLong2Str[6];

	LPSTR lpsHex = strToHex;
	LPSTR lpsLong2Str = strLong2Str;
	LPSTR lpBuffer = strBuffer;
	LPSTR wMessages = strSrciptMessages[wOperation].lpSrcMessOpera;

	sprintf(lpBuffer, ": %s ", wMessages);

	for (wI = 0; wNum = strSrciptMessages[wOperation].szSrcMessOperand[wI]; wI++)
	{
		if (wNum == 0 && wI < iSrciptMessagesChildLen)
			break;

		rgwOperand = pScript->rgwOperand[wI];

		switch ((INT)wNum)
		{

		case kBYTE:
			sprintf(lpBuffer, "%s  %d", lpBuffer, (BYTE)rgwOperand);
			break;

		case kSHORT:
			sprintf(lpBuffer, "%s  %d", lpBuffer, (SHORT)rgwOperand);
			break;

		case kWORD:
			sprintf(lpBuffer, "%s  %d", lpBuffer, rgwOperand);
			break;

		case kBOOL:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, ((INT)rgwOperand) ? "TRUE" : "FALSE");
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, ((INT)rgwOperand) ? "TRUE" : "FALSE");
			break;

		case kEnemyTeamID:
			//sprintf(lpBuffer, "%s  <|", lpBuffer);
			strcat(lpBuffer, "  <|");

			for (wJ = 0; wJ < MAX_ENEMIES_IN_TEAM; wJ++)
			{
				wThisWord = (*pG).lprgEnemyTeam[rgwOperand].rgwEnemy[wJ];
				//sprintf(lpBuffer, "%s%s|", lpBuffer, (wThisWord == 0xFFFF) ? lpZeroWord : PAL_GetWord(wThisWord));
				strcat(lpBuffer, (wThisWord == 0xFFFF || rgwOperand >= (*pG).nEnemyTeam) ? lpZeroWord : ((wThisWord == 0x0000) ? lpNullWord : PAL_GetWord(wThisWord)));

				strcat(lpBuffer, "|");
			}
			//sprintf(lpBuffer, "%s>", lpBuffer);

			strcat(lpBuffer, ">");
			break;

		case kEventID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, (rgwOperand == 0xFFFF) ? lpThisEvent : SpriteID[(*pG).lprgEventObject[rgwOperand].wSpriteNum][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, (rgwOperand == 0xFFFF) ? lpThisEvent : SpriteID[(*pG).lprgEventObject[rgwOperand - 1].wSpriteNum][1]);
			break;

		case kWordID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, PAL_GetWord(pScript->rgwOperand[wI]));
			wThisWord = pScript->rgwOperand[wI];
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, (wThisWord == 0xFFFF) ? lpZeroWord : PAL_GetWord(wThisWord));
			break;

		case kScriptID:
			UTIL_DecToHex(rgwOperand, lpsHex, 4);

			//sprintf(lpBuffer, "%s  %s", lpBuffer, lpsHex);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, lpsHex);
			break;

		case kShopID:
			//sprintf(lpBuffer, "%s  <|", lpBuffer);
			strcat(lpBuffer, "  <|");
			for (wJ = 0; wJ < MAX_STORE_ITEM; wJ++)
			{
				wThisWord = (*pG).lprgStore[rgwOperand].rgwItems[wJ];
				//sprintf(lpBuffer, "%s%s|", lpBuffer, (wThisWord == 0x0000) ? lpZeroWord : PAL_GetWord(wThisWord));
				strcat(lpBuffer, (wThisWord == 0x0000) ? lpZeroWord : PAL_GetWord(wThisWord));

				strcat(lpBuffer, "|");
			}
			//sprintf(lpBuffer, "%s>", lpBuffer);

			strcat(lpBuffer, ">");
			break;

		case kMessageID:
			//sprintf(lpBuffer, "%s  >%s", lpBuffer, PAL_GetMsg(pScript->rgwOperand[wI]));
			strcat(lpBuffer, "  >");

			strcat(lpBuffer, PAL_GetMsg(pScript->rgwOperand[wI]));
			break;

		case kAttributeID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, AttributeID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, AttributeID[rgwOperand][1]);
			break;

		case kBattleResultID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(BattleResultID) / sizeof(BattleResultID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, BattleResultID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, BattleResultID[rgwOperand][1]);
			break;

		case kBattleSpriteID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, BattleSpriteID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, BattleSpriteID[rgwOperand][1]);
			break;

		case kBodyPartID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, BodyPartID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, BodyPartID[rgwOperand][1]);
			break;

		case kCDID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(CDID) / sizeof(CDID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, CDID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, CDID[rgwOperand][1]);
			break;

		case kCharacterID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, CharacterID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, CharacterID[rgwOperand][1]);
			break;

		case kConditionID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, ConditionID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, ConditionID[rgwOperand][1]);
			break;

		case kDirectionID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(DirectionID) / sizeof(DirectionID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, DirectionID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, DirectionID[rgwOperand][1]);
			break;

		case kEnemyDataID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, EnemyDataID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, EnemyDataID[rgwOperand][1]);
			break;

		case kEnemyScriptID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, EnemyScriptID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, EnemyScriptID[rgwOperand][1]);
			break;

		case kFaceID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, FaceID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, FaceID[rgwOperand][1]);
			break;

		case kMagicDataID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, MagicDataID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, MagicDataID[rgwOperand][1]);
			break;

		case kMovieID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, MovieID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, MovieID[rgwOperand][1]);
			break;

		case kMusicID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, MusicID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, MusicID[rgwOperand][1]);
			break;

		case kMusicParamID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, MusicParamID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, MusicParamID[rgwOperand][1]);
			break;

		case kPaletteID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, PaletteID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, PaletteID[rgwOperand][1]);
			break;

		case kPictureID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(PictureID) / sizeof(PictureID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, PictureID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, PictureID[rgwOperand][((gpGlobals->fIsWIN95) ? 2 : 1)]);

			// 获取该战场环境影响
			WORD* lpThisBattleField = (WORD*)&(*pG).lprgBattleField[rgwOperand];

			strcat(lpBuffer, "  <");

			// 拼接战场环境影响基数据
			for (wJ = 0; wJ <= NUM_MAGIC_ELEMENTAL; wJ++)
			{
				sprintf(lpBuffer, "%s|%s: %d", lpBuffer, strElemental[wJ], (SHORT)lpThisBattleField[wJ]);
			}

			strcat(lpBuffer, "|>");
			break;

		case kRoleID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, RoleID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, RoleID[rgwOperand][1]);
			break;

		case kSceneID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(SceneID) / sizeof(SceneID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, SceneID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, SceneID[rgwOperand][1]);
			break;

		case kSpriteID:
			if (rgwOperand == 0xFFFF) rgwOperand = sizeof(SpriteID) / sizeof(SpriteID[0]) - 1;

			//sprintf(lpBuffer, "%s  %s", lpBuffer, SpriteID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, SpriteID[rgwOperand][1]);
			break;

		case kStatusID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, StatusID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, StatusID[rgwOperand][1]);
			break;

		case kTeamMemberID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, TeamMemberID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, TeamMemberID[rgwOperand][1]);
			break;

		case kTriggerMethodID:
			//sprintf(lpBuffer, "%s  %s", lpBuffer, TriggerMethodID[rgwOperand][1]);
			strcat(lpBuffer, "  ");

			strcat(lpBuffer, TriggerMethodID[rgwOperand][1]);
			break;

		default:
			break;

		}
	}

	return lpBuffer;

}

WORD
PAL_GetScriptLen(
	WORD wStartScriptAddress,
	WORD wRoundOfJump
)
{
	WORD wScriptLen = 0;

	GAMEDATA* p = &gpGlobals->g;

	LPSCRIPTENTRY pScript;

	BOOL fEnded = FALSE;

	while (!fEnded)
	{
		pScript = &(*p).lprgScriptEntry[wStartScriptAddress++];

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

		wScriptLen++;
	}

	return wScriptLen;
}
