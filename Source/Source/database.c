#include "../Include/structs_&&_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATABASE_CARDS 500 // safety limit for our card array

// pang check lang heheh
void loadCardDatabase(Card database[], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open card database file: %s\n", filename);
        return;
    }

    char line[256];
    int count = 0;

    // this skips the header row in the cards.csv file in assets folder ("Name,Type,Power...")
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return;
    }

    // reads each line until EOF (wtf does this mean? it means end of file and parses the card data into the database array) HAHAHAHHAHAHAAHAA
    while (fgets(line, sizeof(line), file) != NULL) {
        // safety check to ensure we don't exceed the maximum number of cards blehh
        if (count >= MAX_DATABASE_CARDS - 1) {
            printf("Warning: Database buffer full! Truncating extra cards.\n");
            break;
        }

        char nameBuffer[MAX_NAME];
        int typeVal, powerVal, rarityVal, onceVal, priceVal;

        // parse comma-separated values safely with a width specifier (%31[^,]) (basically it reads up to 31 characters or until a comma is found for clarity)
        if (sscanf(line, "%31[^,],%d,%d,%d,%d,%d", 
                   nameBuffer, &typeVal, &powerVal, &rarityVal, &onceVal, &priceVal) == 6) {
            
            // puts the cards into the database array
            strncpy(database[count].name, nameBuffer, MAX_NAME - 1);
            database[count].name[MAX_NAME - 1] = '\0'; // guarantee null-termination
            database[count].type = (CardType)typeVal;
            database[count].power = powerVal;
            database[count].rarity = rarityVal;
            database[count].is_once_per_encounter = onceVal;
            database[count].price = priceVal;
            database[count].is_active = 1; // Default to available

            count++;
        }
    }

    fclose(file);
    
    //this'll show up in the console when the database is successfully loaded it won't pop in the window tho
    printf("Successfully loaded %d cards from %s into the database.\n", count, filename);
}