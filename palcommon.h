#ifndef _PALUTILS_H
#define _PALUTILS_H

#include "common.h"

#define PAL_DEBUG 0

// 游戏资源目录
#if PAL_DEBUG
#define     lpszPalPath                 "..\\Pal98\\"
#else // PAL_DEBUG
#define     lpszPalPath                 ".\\"
#endif // PAL_DEBUG

#define     lpszPalMassagesPath         lpszPalPath"\\MAKMKF\\"

static LPCSTR lpZeroWord = "无";
static LPCSTR lpNullWord = "空";
static LPCSTR lpThisEvent = "当前对象";

#define     MAX_CONFIG_ID                0x10000

#define     MAX_CONFIG_ID_PARAM_NUM      4

#define     MAX_CONFIG_ID_STRLEN         65

// maximum number of players in party
#define     MAX_PLAYERS_IN_PARTY         3

// total number of possible player roles
#define     MAX_PLAYER_ROLES             6

// totally number of playable player roles
#define     MAX_PLAYABLE_PLAYER_ROLES    5

// maximum entries of inventory
#define     MAX_INVENTORY                256

// maximum items in a store
#define     MAX_STORE_ITEM               9

// total number of magic attributes
#define     NUM_MAGIC_ELEMENTAL          5

// maximum number of enemies in a team
#define     MAX_ENEMIES_IN_TEAM          5

// maximum number of equipments for a player
#define     MAX_PLAYER_EQUIPMENTS        6

// maximum number of magics for a player
#define     MAX_PLAYER_MAGICS            32

// maximum number of scenes
#define     MAX_SCENES                   300

// maximum number of objects
#define     MAX_OBJECTS                  600

// maximum number of event objects (should be somewhat more than the original,
// as there are some modified versions which has more)
#define     MAX_EVENT_OBJECTS            5500

// maximum number of effective poisons to players
#define     MAX_POISONS                  16

// maximum number of level
#define     MAX_LEVELS                   99

#define     MINIMAL_WORD_COUNT           (MAX_OBJECTS + 13)

INT
PAL_MKFGetChunkCount(
   FILE* fp
);

INT
PAL_MKFReadChunk(
   LPBYTE          lpBuffer,
   UINT            uiBufferSize,
   UINT            uiChunkNum,
   FILE* fp
);

INT
PAL_MKFGetChunkSize(
   UINT    uiChunkNum,
   FILE* fp
);

#endif // _PALUTILS_H
