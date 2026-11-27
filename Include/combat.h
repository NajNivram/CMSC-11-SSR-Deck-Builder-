#ifndef COMBAT_H
#define COMBAT_H

#include "structs.h"

/* resets battle state, clears buffs/debuffs, and loads enemy */
void initializeBattle(Battle *battle, int startingHP, const Deck *masterDeck, Fighter enemyData);

/* deals cards from draw pile to hand up to MAX_HAND (10) */
void drawCards(Battle *battle, int count);

/* TAINTS a card from hand/deck mid-fight to instantly draw 4 */
int taintCard(Battle *battle, int handIndex);

/* stages the single turn card from hand onto an open player slot (0-3) */
int playCardToSlot(Battle *battle, int handIndex, int slotIndex);

/* freely returns a staged card from the board back to the hand before lock-in */
int retractCardFromSlot(Battle *battle, int slotIndex);

/* telegraphed enemy action cards onto opposing board slots */
void generateEnemyMove(Battle *battle);

/* resolves slot clashes left-to-right */
void resolveCombatTurn(Battle *battle);

/* evaluates active buff and debuff slots and decrements duration counters */
void applyStatusEffects(Battle *battle);

/* discards spent cards, exiles single-use cards, and advances round counter */
void advanceRound(Battle *battle);

/* assesses win/loss: returns 1 on enemy defeat, 2 on player death, 0 ongoing */
int checkBattleEnd(const Battle *battle);

/* draws the arena, health bars, slots, hand, and sprites using Raylib */
void renderBattleScreen(const Battle *battle);

#endif