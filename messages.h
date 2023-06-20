#ifndef _MESSAGES_H
#define _MESSAGES_H

#include "common.h"
#include "palcommon.h"

typedef enum tagMessage
{
	kBYTE = 1,
	kSHORT,
	kWORD,
	kBOOL,

	kEnemyTeamID,
	kEventID,
	kMessageID,
	kScriptID,
	kShopID,
	kWordID,

	kAttributeID,
	kBattleResultID,
	kBattleSpriteID,
	kBodyPartID,
	kCDID,
	kCharacterID,
	kConditionID,
	kDirectionID,
	kEnemyDataID,
	kEnemyScriptID,
	kFaceID,
	kMagicDataID,
	kMovieID,
	kMusicID,
	kMusicParamID,
	kPaletteID,
	kPictureID,
	kRoleID,
	kSceneID,
	kSpriteID,
	kStatusID,
	kTeamMemberID,
	kTriggerMethodID
} Message;

#endif // #_MESSAGES_H