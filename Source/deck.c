#include "../Include/structs_&&_functions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// intializes the starting deck from that damned csv file
void initializeStartingDeck(Deck *masterDeck, const Card database[], int databaseSize) {
    masterDeck->count = 0;

    // define the starter cards by name, according to Cyan's design
    const char *starterNames[] = {
        "Forward Slash", "Forward Slash",
        "Deep-Cut", 
        "Block", "Block",
        "Self-Heal"
    };

    // this calculates the number of starter cards defined
    int numStarters = sizeof(starterNames) / sizeof(starterNames[0]);

    // this loop goes through each starter card name and finds it in that damned database, then adds it to the master deck
    for (int i = 0; i < numStarters; i++) {
        int found = 0;
        for (int j = 0; j < databaseSize; j++) {
            if (strcmp(database[j].name, starterNames[i]) == 0) {
                // Copy the card from the database into the master deck
                masterDeck->card[masterDeck->count] = database[j];
                masterDeck->count++;
                found = 1;
                break;
            }
        }
        // if the card wasn't found in the database, print a warning in the console not the window, i know it seems redundant but it's a safety check in case the database is missing a card
        if (!found) {
            printf("Warning: Starter card '%s' not found in database!\n", starterNames[i]);
        }
    }

    // Shuffle the newly created starting deck right out of the gate
    shuffleDeck(masterDeck);
    // yeah yeah, i know again, its just for verification
    printf("Successfully initialized starting deck with %d cards.\n", masterDeck->count);
}