#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* were using these as global constants to avoid hardcoding
   values later on (we might add more later on i think)     */
#define BOARD_SLOTS 4
#define MAX_HAND 10
#define MAX_DECK 30
#define MAX_NAME 32

/* the 3 types of cards */
typedef enum {
    CARD_ATTACK,
    CARD_SHIELD,
    CARD_EFFECT
} CardType;

/* Status effect categories */
typedef enum {
    STATUS_NONE = 0,
    STATUS_BUFF,
    STATUS_DEBUFF
} StatusID;

/* Represents an individual active status effect */
typedef struct {
    StatusID id;
    int duration;       /* for how many turns a buff/debuff lasts */
    int value;          /* whatever effect it is like -5hp or +5atk */
} StatusEffect;

/* the struct for our CARDS this one holds the stats for
   each unique card that we will be coming up with       */
typedef struct {
    char name[MAX_NAME];
    CardType type;
    int power;
    int rarity;
    int is_once_per_encounter;
    int is_active;
    int price;
} Card;

/* Fighter struct containing dedicated buff and debuff slots */
typedef struct {
    char name[MAX_NAME];
    int max_hp;
    int current_hp;
    int shield;
    StatusEffect active_buff;   /* this should hold up to 1 active buff */
    StatusEffect active_debuff; /* same for this but for debuff */
} Fighter;

/* this struct represents card collections
   for both the draw and discard pile       */
typedef struct {
    Card card[MAX_DECK];
    int count;
} Deck;

/* this should be the entire state of the game reader */
typedef struct {
    Fighter player;
    Fighter enemy;
    Card player_slots[BOARD_SLOTS];
    Card enemy_slots[BOARD_SLOTS];
    Deck draw_pile;
    Deck discard_pile;
    Card hand[MAX_HAND];
    int hand_count;
    int round_number;
    int next_round_draw;
} Battle;

/* handles generating or presetting the mystery tiles for the current area */
void generateAreaMap(int map[], int floorSize, int areaNumber);

/* prompts the player to select and move to the next connected tile */
int selectNextTile(const int map[], int currentPosition);

/* uncovers/reveals the mystery tile type and branches to combat, shop, camp, or event */
void resolveTileEvent(int tileType, int *playerHP, int *soulPoints, Deck *masterDeck);

/* handles merchant transactions (purchasing new cards or purging cards) heh */
void handleShop(Deck *masterDeck, int *soulPoints);

/* handles camp choices: delete card, pay soul points for whutever (i forgot), or leave */
void handleCamp(Deck *masterDeck, int *playerHP, int maxHP, int *soulPoints, int areaDeckLimit);

/* resolves random text/quiz events and applies outcome rewards/penalties */
void triggerRandomEvent(int eventID, int choiceIndex, int *playerHP, int *soulPoints, Deck *masterDeck);

/* transitions to the next area after defeating a boss (or Area 0 tutorial dummy) */
void advanceToNextArea(int *currentArea, int *currentFloor, int map[], int floorSize);


/* --- functions for battle & decks of cards --- */

/* resets battle state, clears buffs/debuffs, and loads enemy */
void initializeBattle(Battle *battle, int startingHP, const Deck *masterDeck, Fighter enemyData);

/* RNG WOOOOOOOOOOOOOOO, it randomizes card order in the draw pile or recycled discard pile */
void shuffleDeck(Deck *deck);

/* deals cards from draw pile to hand up to MAX_HAND (10) */
void drawCards(Battle *battle, int count);

/* calculates bonus hand draw (+0 atk, +2 shield/effect, +3 single-use) */
int calculateNextDraw(const Card playedCard);

/* TAINTS a card from hand/deck mid-fight to instantly draw 4 */
int taintCard(Battle *battle, int handIndex);


/* --- functions for turn actions andd board staging --- */

/* stages the single turn card from hand onto an open player slot (0-3) */
int playCardToSlot(Battle *battle, int handIndex, int slotIndex);

/* freely returns a staged card from the board back to the hand before lock-in */
int retractCardFromSlot(Battle *battle, int slotIndex);

/* telegraphed enemy action cards onto opposing board slots */
void generateEnemyMove(Battle *battle);


/* --- functions for combat resolution & rtatus mdifiers --- */

/* resolves slot clashes left-to-right (damage absorbs to shield (meron ba?), then HP) */
void resolveCombatTurn(Battle *battle);

/* evaluates active buff and debuff slots and decrements duration counters */
void applyStatusEffects(Battle *battle);


/* --- function for round progression & battle outcome --- */

/* discards spent cards, exiles single-use cards, and advances round counter */
void advanceRound(Battle *battle);

/* assesses win/loss: returns 1 on enemy defeat, 2 on player death, 0 ongoing */
int checkBattleEnd(const Battle *battle);


/* --- functions for edma's art & other files --- */

/* draws the arena, health bars, slots, hand, and sprites using Raylib */
void renderBattleScreen(const Battle *battle);

/* loads card definitions, types, and base stats from an external file (hopefully soon heh) */
void loadCardDatabase(Card database[], const char *filename);
