#ifndef MAP_H
#define MAP_H

#include "structs.h"

#define MAP_STEP_COUNT 15

typedef enum {
    MAP_TILE_START,
    MAP_TILE_COMBAT,
    MAP_TILE_ELITE,
    MAP_TILE_SHOP,
    MAP_TILE_CAMP,
    MAP_TILE_EVENT,
    MAP_TILE_MINIBOSS,
    MAP_TILE_BOSS
} MapTileType;

typedef struct {
    MapTileType top;
    MapTileType bottom;
} MapStep;

/* handles generating or presetting the mystery tiles for the current area */
void generateAreaMap(MapStep map[], int floorSize, int areaNumber);

/* prompts the player to select and move to the next connected tile */
int selectNextTile(const MapStep map[], int currentPosition, int floorSize);

bool mapStepHasBranches(int step);
const char *mapTileLabel(MapTileType tileType);
GameScreen mapTileScreen(MapTileType tileType);

/* uncovers/reveals the mystery tile type and branches to combat, shop, camp, or event */
void resolveTileEvent(MapTileType tileType, int *playerHP, int *soulPoints, Deck *masterDeck);

/* handles merchant transactions (purchasing new cards or purging cards) */
void handleShop(Deck *masterDeck, int *soulPoints);

/* handles camp choices: delete card, heal, or leave */
void handleCamp(Deck *masterDeck, int *playerHP, int maxHP, int *soulPoints, int areaDeckLimit);

/* resolves random text/quiz events and applies outcome rewards/penalties */
void triggerRandomEvent(int eventID, int choiceIndex, int *playerHP, int *soulPoints, Deck *masterDeck);

/* transitions to the next area after defeating a boss */
void advanceToNextArea(int *currentArea, int *currentFloor, MapStep map[], int floorSize);

#endif