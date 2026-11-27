#ifndef DECK_H
#define DECK_H

#include "structs.h"

/* RNG card randomization in draw pile or recycled discard pile */
void shuffleDeck(Deck *deck);

/* initializes starting deck from csv database */
void initializeStartingDeck(Deck *masterDeck, const Card database[], int databaseSize);

/* calculates bonus hand draw (+0 atk, +2 shield/effect, +3 single-use) */
int calculateNextDraw(const Card playedCard);

#endif