#include "../Include/structs.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>

// starting here this handles map generation and navigation(like player movement and many more)

static MapTileType generateRandomTile(void) {
    const int roll = rand() % 100;
    if (roll < 60) return MAP_TILE_COMBAT;
    if (roll < 80) return MAP_TILE_SHOP;
    return MAP_TILE_EVENT;
}

bool mapStepHasBranches(int step) {
    return (step >= 1 && step <= 5) || (step >= 8 && step <= 12);
}

void generateAreaMap(MapStep map[], int floorSize, int areaNumber) {
    (void)areaNumber;

    for (int step = 0; step < floorSize; step++) {
        if (step == 0) {
            map[step].top = MAP_TILE_START;
            map[step].bottom = MAP_TILE_START;
        } else if (step == 6 || step == 13) {
            map[step].top = MAP_TILE_CAMP;
            map[step].bottom = MAP_TILE_CAMP;
        } else if (step == 7) {
            map[step].top = MAP_TILE_MINIBOSS;
            map[step].bottom = MAP_TILE_MINIBOSS;
        } else if (step == 14) {
            map[step].top = MAP_TILE_BOSS;
            map[step].bottom = MAP_TILE_BOSS;
        } else {
            map[step].top = generateRandomTile();
            map[step].bottom = generateRandomTile();
        }
    }
}

// handles the players movement through the map,
// returning the next tile index based on the current
// position and ensuring it doesn't exceed the bounds of the map array
int selectNextTile(const MapStep map[], int currentPosition, int floorSize) {
    if (map == NULL || currentPosition < 0 || currentPosition >= floorSize) {
        return -1;
    }

    return currentPosition + 1;
}

const char *mapTileLabel(MapTileType tileType) {
    switch (tileType) {
        case MAP_TILE_START: return "START";
        case MAP_TILE_COMBAT: return "COMBAT";
        case MAP_TILE_ELITE: return "ELITE";
        case MAP_TILE_SHOP: return "SHOP";
        case MAP_TILE_CAMP: return "CAMP";
        case MAP_TILE_EVENT: return "EVENT";
        case MAP_TILE_MINIBOSS: return "MINIBOSS";
        case MAP_TILE_BOSS: return "BOSS";
        default: return "UNKNOWN";
    }
}

GameScreen mapTileScreen(MapTileType tileType) {
    switch (tileType) {
        case MAP_TILE_START: return SCREEN_GAMEPLAY;
        case MAP_TILE_COMBAT:
        case MAP_TILE_ELITE:
        case MAP_TILE_MINIBOSS:
        case MAP_TILE_BOSS:
            return SCREEN_BATTLE;
        case MAP_TILE_SHOP:
            return SCREEN_SHOP;
        case MAP_TILE_CAMP:
            return SCREEN_CAMP;
        case MAP_TILE_EVENT:
            return SCREEN_EVENT;
        default:
            return SCREEN_MAP;
    }
}

// STARTING DOWN HERE THIS HANDLES THE SHOP AND CAMP MECHANICS and all that good stuff
// this is where we handle the shop and camp mechanics,
// including card rarity rolls, purchases, and player choices

// helper function to roll a card rarity based on Cyan's cascading table (5-star down to 1-star)
static int roll_card_rarity(void) {
    // 5 star check (1 in 4096 chance)
    if ((rand() % 4096) == 0) return 5;
    // 4 star check (1 in 1000 chance)
    if ((rand() % 1000) == 0) return 4;
    // 3 star check (1 in 500 chance)
    if ((rand() % 500) == 0) return 3;
    // 2 star check (1 in 250 chance)
    if ((rand() % 250) == 0) return 2;
    // default / 1 star check (1 in 100 chance)
    return 1;
}

// handles the merchant shop where items are rolled by rarity and prices scale accordingly
void handleShop(Deck *masterDeck, int *soulPoints) {
    // static counter so rerolls remember how many times we rolled (first 3 are free!)
    static int rerollCount = 0;
    
    // figure out reroll cost based on Cyan's rules
    int rerollCost = (rerollCount < 3) ? 0 : 5; // costs 5 soul points after the first 3 free rolls

    // rolls 2 random cards from the database to stock the shop inventory
    Card shopStock[2];
    for (int i = 0; i < 2; i++) {
        int targetRarity = roll_card_rarity();
        
        // find a random card in database matching this rarity
        int foundCard = 0;
        while (!foundCard) {
            // grab a fallback or matched card safely from master deck/database pool
            shopStock[i] = masterDeck->card[rand() % masterDeck->count]; 
            foundCard = 1;
        }
    }

    // TODO in UI: Display shopStock[0] and shopStock[1], handle buying and rerolling here!
}

// handles camp choices: exile cards, heal, or leave (guarded strictly by the area deck limit)
void handleCamp(Deck *masterDeck, int *playerHP, int maxHP, int *soulPoints, int areaDeckLimit) {
    int inCamp = 1;

    while (inCamp) {
        // player gets 3 options at camp: exile cards, heal, or leave
        
        // option 1: exile cards (permanently remove card from masterDeck, gain soul points)
        
        // option 2: heal (pay soul points to restore playerHP up to maxHP)
        
        // option 3: leave (this has Jade's strict guardrail!)
        // you can ONLY leave if your deck count is under or equal to the area limit
        if (masterDeck->count <= areaDeckLimit) {
            inCamp = 0; // successfully exit camp loop
        } else {
            // block leaving until cards are pruned!
            // printf("Deck size (%d) exceeds area limit (%d)! Exile cards before leaving.\n", 
            //        masterDeck->count, areaDeckLimit);
            break; // breaking for safety in backend stub
        }
    }
}

// TILE ROUTING BS AND EVENT HANDLING:
// in seriousness this is where we route the player to the 
// correct game subsystem based on the tile type they landed on,
// and handle random events that can occur in the game

// routes the player to the correct game subsystem depending on the tile type they landed on
void resolveTileEvent(MapTileType tileType, int *playerHP, int *soulPoints, Deck *masterDeck) {
    switch (tileType) {
        case MAP_TILE_COMBAT:
        case MAP_TILE_ELITE:
        case MAP_TILE_MINIBOSS:
        case MAP_TILE_BOSS:
            // combat tiles trigger battle encounters (managed via combat.c)
            break;
            
        case MAP_TILE_SHOP:
            handleShop(masterDeck, soulPoints);
            break;
            
        case MAP_TILE_CAMP:
            // default maxHP set to 100 and area deck limit set to 20 for backend safety
            handleCamp(masterDeck, playerHP, 100, soulPoints, 20);
            break;
            
        case MAP_TILE_EVENT:
            // triggers random mystery events (placeholder event 1, choice 1)
            triggerRandomEvent(1, 1, playerHP, soulPoints, masterDeck);
            break;
            
        default:
            break;
    }
}

// handles mystery events that can replace intermediate tiles, offering choices and outcomes (like card trades or HP gambles)
void triggerRandomEvent(int eventID, int choiceIndex, int *playerHP, int *soulPoints, Deck *masterDeck) {
    // switch case for different random event scenarios based on Cyan's design doc
    switch (eventID) {
        case 1:
            // e.g., Strange Altar: trade 10 health for 20 soul points
            if (choiceIndex == 1) {
                *playerHP -= 10;
                *soulPoints += 20;
            }
            break;
            
        default:
            break;
    }
}

// handles moving from one completed area to the next after defeating the area boss
void advanceToNextArea(int *currentArea, int *currentFloor, MapStep map[], int floorSize) {
    // bump up the area difficulty counter
    (*currentArea)++;
    
    // reset current floor back to the beginning
    *currentFloor = 0;
    
    // generate a brand new map layout for the new area
    generateAreaMap(map, floorSize, *currentArea);
}