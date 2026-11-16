#include "raylib.h"
#include "../Include/structs_&&_functions.h"

#define MAX_DATABASE_CARDS 500 // new safety limit for our card array CUZ CYAN HUMOROUSLY SAID 500

// this is the main function that initializes the window and loads the card database
int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "TempWindow");
    SetTargetFPS(60);

    // this is for loading the card database "cards.csv"
    Card cardDatabase[MAX_DATABASE_CARDS];
    loadCardDatabase(cardDatabase, "Assets/cards.csv");

    // this is the main game loop that continues until the window is closed
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // start with the main menu or deck builder screen here

        EndDrawing();
    }

    CloseWindow();

    return 0;
}