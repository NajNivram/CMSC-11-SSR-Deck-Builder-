#include "../Include/structs_&&_functions.h"
#include <stdlib.h>

/* RNG Fisher-Yates shuffle algorithm for the deck that i got on the internet */
void shuffleDeck(Deck *deck) {
    if (deck->count <= 1) return;
    
    for (int i = deck->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->card[i];
        deck->card[i] = deck->card[j];
        deck->card[j] = temp;
    }
}

/* deals cards from the draw pile into the hand */
void drawCards(Battle *battle, int count) {
    for (int i = 0; i < count; i++) {
        // checks if we already have 10 cards (MAX_HAND is 10)
        if (battle->hand_count >= MAX_HAND) {
            break;
        }
        
        // if draw pile is empty, recycle discard pile
        if (battle->draw_pile.count == 0) {
            if (battle->discard_pile.count == 0) {
                break; // no cards left anywhere to draw
            }
            battle->draw_pile = battle->discard_pile;
            battle->discard_pile.count = 0;
            shuffleDeck(&battle->draw_pile);
        }
        
        // draw the top card into the hand
        battle->hand[battle->hand_count] = battle->draw_pile.card[battle->draw_pile.count - 1];
        battle->draw_pile.count--;
        battle->hand_count++;
    }
}