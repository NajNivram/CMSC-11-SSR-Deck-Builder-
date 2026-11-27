#ifndef MAP_H
#define MAP_H

#include "structs.h"

/* handles generating or presetting the mystery tiles for the current area */
void generateAreaMap(int map[], int floorSize, int areaNumber);

/* prompts the player to select and move to the next connected tile */
int selectNextTile(const int map[], int currentPosition);

/* uncovers/reveals the mystery tile type and branches to combat, shop, camp, or event */
void resolveTileEvent(int tileType, int *playerHP, int *soulPoints, Deck *masterDeck);

/* handles merchant transactions (purchasing new cards or purging cards) */
void handleShop(Deck *masterDeck, int *soulPoints);

/* handles camp choices: delete card, heal, or leave */
void handleCamp(Deck *masterDeck, int *playerHP, int maxHP, int *soulPoints, int areaDeckLimit);

/* resolves random text/quiz events and applies outcome rewards/penalties */
void triggerRandomEvent(int eventID, int choiceIndex, int *playerHP, int *soulPoints, Deck *masterDeck);

/* transitions to the next area after defeating a boss */
void advanceToNextArea(int *currentArea, int *currentFloor, int map[], int floorSize);

#endif