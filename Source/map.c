#include "../Include/structs_&&_functions.h"
#include <stdlib.h>
#include <time.h>

// this assigns the tile types to the map array based on the floor size and area number, ensuring a mix of combat, shop, event, and milestone tiles
typedef enum {
    TILE_EMPTY = 0,
    TILE_COMBAT,    // Standard fight
    TILE_ELITE,     // Elite fight
    TILE_SHOP,      // Merchant shop
    TILE_CAMP,      // Campfire / Rest
    TILE_EVENT,     // Random mystery event
    TILE_MINIBOSS,  // Mini-boss fight
    TILE_BOSS       // Area Boss
} TileType;

// starting here this handles map generation and navigation(like player movement and many more)

// will build our map array for the current area, filling it with the appropriate tile types based on the floor size and area number
void generateAreaMap(int map[], int floorSize, int areaNumber) {
    for (int i = 0; i < floorSize; i++) {
        
        // 1. checks if it's the final tile (or the Area Boss)
        if (i == floorSize - 1) {
            map[i] = TILE_BOSS;
        }
        // 2. cecks if it's a 10th tile milestone (the Mini-Boss)
        else if ((i + 1) % 10 == 0) {
            map[i] = TILE_MINIBOSS;
        }
        // 3. check if it's a 5th or 15th tile milestone (Camp)
        else if ((i + 1) % 5 == 0) {
            map[i] = TILE_CAMP;
        }
        else {
            // Randomly roll intermediate tile types (Combat, Shop, or Event)
            int roll = rand() % 100;
            if (roll < 60) {
                map[i] = TILE_COMBAT; // 60% chance for standard combat
            } else if (roll < 80) {
                map[i] = TILE_SHOP;   // 20% chance for a shop
            } else {
                map[i] = TILE_EVENT;  // 20% chance for a random event
            }
        }
    }
}

// handles the players movement through the map,
// returning the next tile index based on the current
// position and ensuring it doesn't exceed the bounds of the map array
int selectNextTile(const int map[], int currentPosition) {
    int nextPosition = currentPosition + 1;
    
    // bound checker: makes sure we don't walk past the end of the map array
    // (Assuming floorSize is handled or passed, but a safe array limit check can go here)
    if (nextPosition < 0) {
        return -1;
    }
    
    return nextPosition;
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
void resolveTileEvent(int tileType, int *playerHP, int *soulPoints, Deck *masterDeck) {
    switch (tileType) {
        case TILE_COMBAT:
        case TILE_ELITE:
        case TILE_MINIBOSS:
        case TILE_BOSS:
            // combat tiles trigger battle encounters (managed via combat.c)
            break;
            
        case TILE_SHOP:
            handleShop(masterDeck, soulPoints);
            break;
            
        case TILE_CAMP:
            // default maxHP set to 100 and area deck limit set to 20 for backend safety
            handleCamp(masterDeck, playerHP, 100, soulPoints, 20);
            break;
            
        case TILE_EVENT:
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
void advanceToNextArea(int *currentArea, int *currentFloor, int map[], int floorSize) {
    // bump up the area difficulty counter
    (*currentArea)++;
    
    // reset current floor back to the beginning
    *currentFloor = 0;
    
    // generate a brand new map layout for the new area
    generateAreaMap(map, floorSize, *currentArea);
}