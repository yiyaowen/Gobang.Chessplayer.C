#ifndef GAME_PREFAB_CONFIG_H
#define GAME_PREFAB_CONFIG_H

typedef int GameMode;
typedef int GameOrder;
typedef int GameLevel;

typedef struct type_GamePrefabConfig {
    GameMode mode;
    GameOrder order;
    GameLevel level;
} GamePrefabConfig;

GamePrefabConfig* getGamePrefabConfig(GameMode mode, GameOrder order, GameLevel level);

#define getPageIdFromGameMode(MODE) ((MODE) == GAME_MODE_PVP ? PAGE_ID_PVP : PAGE_ID_PVC)
#define getPageNameFromGameMode(MODE) ((MODE) == GAME_MODE_PVP ? PAGE_NAME_PVP : PAGE_NAME_PVC)
#define getPageDescFromGameMode(MODE) ((MODE) == GAME_MODE_PVP ? PAGE_DESC_PVP : PAGE_DESC_PVC)

void releaseGamePrefabConfig(GamePrefabConfig** config);

/* Game mode */
#define GAME_MODE_PVP    0
#define GAME_MODE_PVC    1

/* Game order */
#define GAME_ORDER_PLAYER_FIRST          10
#define GAME_ORDER_CHESSPLAYER_FIRST     20

/* Game level */
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
/* Oops! Chessplayer is drunk. You win for sure. */
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
#define GAME_LEVEL_DRUNK  -100

#define GAME_LEVEL_LOW      0
#define GAME_LEVEL_MIDDLE   1
#define GAME_LEVEL_HIGH     2

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
/* You lose for sure. Be happy. :-) */
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
#define GAME_LEVEL_INVINCIBLE    100

#define GAME_LEVEL_RANDOM    25

#endif // GAME_PREFAB_CONFIG_H