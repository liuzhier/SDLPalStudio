#ifndef GLOBAL_H
#define GLOBAL_H

#include "common.h"
#include "palcommon.h"
#include "messages.c"

typedef enum tagSCRIPTTYPE
{
	kScriptOnEnter = 0,
	kScriptOnTeleport = 1,
} SCRIPTTYPE;

typedef struct tagFILES
{
	FILE* fpFBP;      // battlefield background images
	FILE* fpMGO;      // sprites in scenes
	FILE* fpBALL;     // item bitmaps
	FILE* fpDATA;     // misc data
	FILE* fpF;        // player sprites during battle
	FILE* fpFIRE;     // fire effect sprites
	FILE* fpRGM;      // character face bitmaps
	FILE* fpSSS;      // script data
} FILES, * LPFILES;

typedef struct tagEVENTOBJECT
{
	SHORT        sVanishTime;         // vanish time (?) 一般用于怪物这种触发战斗的事件对象，战后设置隐藏该事件对象的时间
	WORD         x;                   // X coordinate on the map 地图上的X坐标
	WORD         y;                   // Y coordinate on the map 地图上的Y坐标
	SHORT        sLayer;              // layer value 图层
	WORD         wTriggerScript;      // Trigger script entry 调查脚本
	WORD         wAutoScript;         // Auto script entry 自动脚本
	SHORT        sState;              // state of this object 事件状态（0=隐藏 1=飘浮 2=实体(碰撞检测) 3=特殊状态(一般用作某些对象脚本的开关)）
	WORD         wTriggerMode;        // trigger mode 触发方式（有自动触发还有调查触发）
	WORD         wSpriteNum;          // number of the sprite 形象号
	USHORT       nSpriteFrames;       // total number of frames of the sprite 每个方向的图像帧数
	WORD         wDirection;          // direction 事件面朝的方向
	WORD         wCurrentFrameNum;    // current frame number 当前帧数是当前方向图像序列的第几帧
	USHORT       nScriptIdleFrame;    // count of idle frames, used by trigger script
	WORD         wSpritePtrOffset;    // FIXME: ???
	USHORT       nSpriteFramesAuto;   // total number of frames of the sprite, used by auto script
	WORD         wScriptIdleFrameCountAuto;     // count of idle frames, used by auto script
} EVENTOBJECT, * LPEVENTOBJECT;

typedef struct tagSCENE
{
	WORD         wMapNum;         // number of the map
	WORD         wScriptOnEnter;  // when entering this scene, execute script from here
	WORD         wScriptOnTeleport;  // when teleporting out of this scene, execute script from here
	WORD         wEventObjectIndex;  // event objects in this scene begins from number wEventObjectIndex + 1
} SCENE, * LPSCENE;

// system strings and players
typedef struct tagOBJECT_PLAYER
{
	WORD         wReserved[2];    // always zero
	WORD         wScriptOnFriendDeath; // when friends in party dies, execute script from here
	WORD         wScriptOnDying;  // when dying, execute script from here
} OBJECT_PLAYER;

typedef enum tagITEMFLAG
{
	kItemFlagUsable = (1 << 0),
	kItemFlagEquipable = (1 << 1),
	kItemFlagThrowable = (1 << 2),
	kItemFlagConsuming = (1 << 3),
	kItemFlagApplyToAll = (1 << 4),
	kItemFlagSellable = (1 << 5),
	kItemFlagEquipableByPlayerRole_First = (1 << 6)
} ITEMFLAG;


// items
typedef struct tagOBJECT_ITEM_DOS
{
	WORD         wBitmap;         // bitmap number in BALL.MKF
	WORD         wPrice;          // price
	WORD         wScriptOnUse;    // script executed when using this item
	WORD         wScriptOnEquip;  // script executed when equipping this item
	WORD         wScriptOnThrow;  // script executed when throwing this item to enemy
	WORD         wFlags;          // flags
} OBJECT_ITEM_DOS;

// items
typedef struct tagOBJECT_ITEM
{
	WORD         wBitmap;         // bitmap number in BALL.MKF
	WORD         wPrice;          // price
	WORD         wScriptOnUse;    // script executed when using this item
	WORD         wScriptOnEquip;  // script executed when equipping this item
	WORD         wScriptOnThrow;  // script executed when throwing this item to enemy
	WORD         wScriptDesc;     // description script
	WORD         wFlags;          // flags
} OBJECT_ITEM;

typedef enum tagMAGICFLAG
{
	kMagicFlagUsableOutsideBattle = (1 << 0),
	kMagicFlagUsableInBattle = (1 << 1),
	kMagicFlagUsableToEnemy = (1 << 3),
	kMagicFlagApplyToAll = (1 << 4),
} MAGICFLAG;

// magics
typedef struct tagOBJECT_MAGIC_DOS
{
	WORD         wMagicNumber;      // magic number, according to DATA.MKF #3
	WORD         wReserved1;        // always zero
	WORD         wScriptOnSuccess;  // when magic succeed, execute script from here
	WORD         wScriptOnUse;      // when use this magic, execute script from here
	WORD         wReserved2;        // always zero
	WORD         wFlags;            // flags
} OBJECT_MAGIC_DOS;

// magics
typedef struct tagOBJECT_MAGIC
{
	WORD         wMagicNumber;      // magic number, according to DATA.MKF #3
	WORD         wReserved1;        // always zero
	WORD         wScriptOnSuccess;  // when magic succeed, execute script from here
	WORD         wScriptOnUse;      // when use this magic, execute script from here
	WORD         wScriptDesc;       // description script
	WORD         wReserved2;        // always zero
	WORD         wFlags;            // flags
} OBJECT_MAGIC;

// enemies
typedef struct tagOBJECT_ENEMY
{
	WORD         wEnemyID;        // ID of the enemy, according to DATA.MKF #1.
	// Also indicates the bitmap number in ABC.MKF.
	WORD         wResistanceToSorcery;  // resistance to sorcery and poison (0 min, 10 max)
	WORD         wScriptOnTurnStart;    // script executed when turn starts
	WORD         wScriptOnBattleEnd;    // script executed when battle ends
	WORD         wScriptOnReady;        // script executed when the enemy is ready
} OBJECT_ENEMY;

// poisons (scripts executed in each round)
typedef struct tagOBJECT_POISON
{
	WORD         wPoisonLevel;    // level of the poison
	WORD         wColor;          // color of avatars
	WORD         wPlayerScript;   // script executed when player has this poison (per round)
	WORD         wReserved;       // always zero
	WORD         wEnemyScript;    // script executed when enemy has this poison (per round)
} OBJECT_POISON;

typedef union tagOBJECT_DOS
{
	WORD              rgwData[6];
	OBJECT_PLAYER     player;
	OBJECT_ITEM_DOS   item;
	OBJECT_MAGIC_DOS  magic;
	OBJECT_ENEMY      enemy;
	OBJECT_POISON     poison;
} OBJECT_DOS, * LPOBJECT_DOS;

typedef union tagOBJECT
{
	WORD              rgwData[7];
	OBJECT_PLAYER     player;
	OBJECT_ITEM       item;
	OBJECT_MAGIC      magic;
	OBJECT_ENEMY      enemy;
	OBJECT_POISON     poison;
} OBJECT, * LPOBJECT;

// object including system strings, players, items, magics, enemies and poison scripts.

typedef struct tagSCRIPTENTRY
{
	WORD          wOperation;     // operation code
	WORD          rgwOperand[3];  // operands
} SCRIPTENTRY, * LPSCRIPTENTRY;

typedef struct tagSTORE
{
	WORD          rgwItems[MAX_STORE_ITEM];
} STORE, * LPSTORE;

typedef struct tagENEMY
{
	WORD        wIdleFrames;         // total number of frames when idle
	WORD        wMagicFrames;        // total number of frames when using magics
	WORD        wAttackFrames;       // total number of frames when doing normal attack
	WORD        wIdleAnimSpeed;      // speed of the animation when idle
	WORD        wActWaitFrames;      // FIXME: ???
	WORD        wYPosOffset;
	SHORT       wAttackSound;        // sound played when this enemy uses normal attack
	SHORT       wActionSound;        // FIXME: ???
	SHORT       wMagicSound;         // sound played when this enemy uses magic
	SHORT       wDeathSound;         // sound played when this enemy dies
	SHORT       wCallSound;          // sound played when entering the battle
	WORD        wHealth;             // total HP of the enemy
	WORD        wExp;                // How many EXPs we'll get for beating this enemy
	WORD        wCash;               // how many cashes we'll get for beating this enemy
	WORD        wLevel;              // this enemy's level
	WORD        wMagic;              // this enemy's magic number
	WORD        wMagicRate;          // chance for this enemy to use magic
	WORD        wAttackEquivItem;    // equivalence item of this enemy's normal attack
	WORD        wAttackEquivItemRate;// chance for equivalence item
	WORD        wStealItem;          // which item we'll get when stealing from this enemy
	WORD        nStealItem;          // total amount of the items which can be stolen
	WORD        wAttackStrength;     // normal attack strength
	WORD        wMagicStrength;      // magical attack strength
	WORD        wDefense;            // resistance to all kinds of attacking
	WORD        wDexterity;          // dexterity
	WORD        wFleeRate;           // chance for successful fleeing
	WORD        wPoisonResistance;   // resistance to poison
	WORD        wElemResistance[NUM_MAGIC_ELEMENTAL]; // resistance to elemental magics
	WORD        wPhysicalResistance; // resistance to physical attack
	WORD        wDualMove;           // whether this enemy can do dual move or not
	WORD        wCollectValue;       // value for collecting this enemy for items
} ENEMY, * LPENEMY;

typedef struct tagENEMYTEAM
{
	WORD        rgwEnemy[MAX_ENEMIES_IN_TEAM];
} ENEMYTEAM, * LPENEMYTEAM;

typedef WORD PLAYERS[MAX_PLAYER_ROLES];

typedef struct tagPLAYERROLES
{
	PLAYERS            rgwAvatar;             // avatar (shown in status view)
	PLAYERS            rgwSpriteNumInBattle;  // sprite displayed in battle (in F.MKF)
	PLAYERS            rgwSpriteNum;          // sprite displayed in normal scene (in MGO.MKF)
	PLAYERS            rgwName;               // name of player class (in WORD.DAT)
	PLAYERS            rgwAttackAll;          // whether player can attack everyone in a bulk or not
	PLAYERS            rgwUnknown1;           // FIXME: ???
	PLAYERS            rgwLevel;              // level
	PLAYERS            rgwMaxHP;              // maximum HP
	PLAYERS            rgwMaxMP;              // maximum MP
	PLAYERS            rgwHP;                 // current HP
	PLAYERS            rgwMP;                 // current MP
	WORD               rgwEquipment[MAX_PLAYER_EQUIPMENTS][MAX_PLAYER_ROLES]; // equipments
	PLAYERS            rgwAttackStrength;     // normal attack strength
	PLAYERS            rgwMagicStrength;      // magical attack strength
	PLAYERS            rgwDefense;            // resistance to all kinds of attacking
	PLAYERS            rgwDexterity;          // dexterity
	PLAYERS            rgwFleeRate;           // chance of successful fleeing
	PLAYERS            rgwPoisonResistance;   // resistance to poison
	WORD               rgwElementalResistance[NUM_MAGIC_ELEMENTAL][MAX_PLAYER_ROLES]; // resistance to elemental magics
	PLAYERS            rgwUnknown2;           // FIXME: ???
	PLAYERS            rgwUnknown3;           // FIXME: ???
	PLAYERS            rgwUnknown4;           // FIXME: ???
	PLAYERS            rgwCoveredBy;          // who will cover me when I am low of HP or not sane
	WORD               rgwMagic[MAX_PLAYER_MAGICS][MAX_PLAYER_ROLES]; // magics
	PLAYERS            rgwWalkFrames;         // walk frame (???)
	PLAYERS            rgwCooperativeMagic;   // cooperative magic
	PLAYERS            rgwUnknown5;           // FIXME: ???
	PLAYERS            rgwUnknown6;           // FIXME: ???
	PLAYERS            rgwDeathSound;         // sound played when player dies
	PLAYERS            rgwAttackSound;        // sound played when player attacks
	PLAYERS            rgwWeaponSound;        // weapon sound (???)
	PLAYERS            rgwCriticalSound;      // sound played when player make critical hits
	PLAYERS            rgwMagicSound;         // sound played when player is casting a magic
	PLAYERS            rgwCoverSound;         // sound played when player cover others
	PLAYERS            rgwDyingSound;         // sound played when player is dying
} PLAYERROLES, * LPPLAYERROLES;

typedef struct tagMAGIC
{
	WORD               wEffect;               // effect sprite
	WORD               wType;                 // type of this magic
	WORD               wXOffset;
	WORD               wYOffset;
	WORD               wSummonEffect;         // summon effect sprite (in F.MKF)
	SHORT              wSpeed;                // speed of the effect
	WORD               wKeepEffect;           // FIXME: ???
	WORD               wFireDelay;            // start frame of the magic fire stage
	WORD               wEffectTimes;          // total times of effect
	WORD               wShake;                // shake screen
	WORD               wWave;                 // wave screen
	WORD               wUnknown;              // FIXME: ???
	WORD               wCostMP;               // MP cost
	WORD               wBaseDamage;           // base damage
	WORD               wElemental;            // elemental (0 = No Elemental, last = poison)
	SHORT              wSound;                // sound played when using this magic
} MAGIC, * LPMAGIC;

typedef struct tagBATTLEFIELD
{
	WORD               wScreenWave;                      // level of screen waving
	SHORT              rgsMagicEffect[NUM_MAGIC_ELEMENTAL]; // effect of attributed magics
} BATTLEFIELD, * LPBATTLEFIELD;

// magics learned when level up
typedef struct tagLEVELUPMAGIC
{
	WORD               wLevel;    // level reached
	WORD               wMagic;    // magic learned
} LEVELUPMAGIC, * LPLEVELUPMAGIC;

typedef struct tagLEVELUPMAGIC_ALL
{
	LEVELUPMAGIC       m[MAX_PLAYABLE_PLAYER_ROLES];
} LEVELUPMAGIC_ALL, * LPLEVELUPMAGIC_ALL;

typedef struct tagPALPOS
{
	WORD      x;
	WORD      y;
} PALPOS, * LPPALPOS;

typedef struct tagENEMYPOS
{
	PALPOS pos[MAX_ENEMIES_IN_TEAM][MAX_ENEMIES_IN_TEAM];
} ENEMYPOS, * LPENEMYPOS;

// Exp. points needed for the next level
typedef WORD LEVELUPEXP, * LPLEVELUPEXP;

// game data which is available in data files.
// 数据文件中可用的游戏数据
typedef struct tagGAMEDATA
{
	LPEVENTOBJECT           lprgEventObject;
	int                     nEventObject;

	SCENE                   rgScene[MAX_SCENES];
	OBJECT                  rgObject[MAX_OBJECTS];

	LPSCRIPTENTRY           lprgScriptEntry;
	int                     nScriptEntry;

	LPSTORE                 lprgStore;
	int                     nStore;

	LPENEMY                 lprgEnemy;
	int                     nEnemy;

	LPENEMYTEAM             lprgEnemyTeam;
	int                     nEnemyTeam;

	PLAYERROLES             PlayerRoles;

	LPMAGIC                 lprgMagic;
	int                     nMagic;

	LPBATTLEFIELD           lprgBattleField;
	int                     nBattleField;

	LPLEVELUPMAGIC_ALL      lprgLevelUpMagic;
	int                     nLevelUpMagic;

	ENEMYPOS                EnemyPos;
	LEVELUPEXP              rgLevelUpExp[MAX_LEVELS + 1];

	WORD                    rgwBattleEffectIndex[10][2];
} GAMEDATA, * LPGAMEDATA;

typedef struct tagTEXTENTRYMESSAGE
{
	CHAR             strScrpitEntry[256];
} TEXTENTRYMESSAGE, * LPTEXTENTRYMESSAGE;

typedef struct tagGLOBALVARS
{
	FILES            f;
	GAMEDATA         g;

	BOOL             fIsWIN95;
} GLOBALVARS, * LPGLOBALVARS;

PAL_C_LINKAGE_BEGIN

extern GLOBALVARS* const gpGlobals;

// 当前脚本地址
static WORD wStartScriptAddress = 0, wJumpRound = 0;

BOOL
PAL_IsWINVersion(
	VOID
);

VOID
PAL_LoadGameData(
	VOID
);

INT
PAL_InitGlobals(
	VOID
);

VOID
PAL_FreeGlobals(
	VOID
);

VOID
PAL_InitIDCfg(
	VOID
);

PAL_C_LINKAGE_END

#endif //GLOBAL_H