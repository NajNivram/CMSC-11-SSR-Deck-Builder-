#include "raylib.h"
#include "../Include/structs_&&_functions.h"

#define MAX_DATABASE_CARDS 100

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "TempWindow");
    
    SetTargetFPS(60);

    // this is for loading the card database "cards.csv"
    Card cardDatabase[MAX_DATABASE_CARDS];
    loadCardDatabase(cardDatabase, "Assets/cards.csv");

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
