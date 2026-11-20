#include "raylib.h"
#include <stdbool.h>
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

    // for the deckkkkkkkkkkkkkkk of the player, we initialize it with the starting cards from the database/csv file heh
    Deck playerMasterDeck;
    initializeStartingDeck(&playerMasterDeck, cardDatabase, MAX_DATABASE_CARDS);

    //hero stuff
    Fighter MC = {"guy", 50, 50};

    //buttons
    Rectangle PlayBtn = { 1000, 450, 200, 60 };
    Rectangle ExitBtn = { 1000, 550, 200, 60 };

    //drawing positions
    int rectWidth = screenWidth;
    int rectHeight = screenHeight / 2;
    int posX = (GetScreenWidth() - rectWidth) / 2;   // Horizontal center
    int posY = rectHeight;

    Vector2 HeroPosition = { 1.0f , 110.0f };
    
    //sprites
    Texture2D background = LoadTexture("Assets/Sprites/ui elements/StartScreen.png");
    Texture2D logo = LoadTexture("Assets/Sprites/ui elements/StartScreenLogo.png");
    Texture2D Hero = LoadTexture("Assets/Sprites/placeholder assets/p_idl_kurokami.png");


    //our button
    bool DrawButton(Rectangle bounds, const char *text, int fontSize, Color baseColor, Color hoverColor, Color pressColor, Color textColor) {
    Vector2 mousePoint = GetMousePosition();
    bool isHovered = CheckCollisionPointRec(mousePoint, bounds);
    bool isClicked = false;

    Color currentColor = baseColor;

    if (isHovered) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            currentColor = pressColor;
        } else {
            currentColor = hoverColor;
        }

        // Trigger action on release (standard UI behavior)
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isClicked = true;
        }
    }

    // Draw button background and border
    DrawRectangleRec(bounds, currentColor);
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    // Center text inside the button
    int textWidth = MeasureText(text, fontSize);
    int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
    int textY = (int)(bounds.y + (bounds.height - fontSize) / 2);
    DrawText(text, textX, textY, fontSize, textColor);

    return isClicked;
    }

    GameScreen currentScreen = SCREEN_MENU;
    
    // this is the main game loop that continues until the window is closed
    while (!WindowShouldClose())
    {
        if (currentScreen == SCREEN_MENU) {
        BeginDrawing();
        ClearBackground(BLUE);
        
        // start with the main menu or deck builder screen here

        Rectangle source = { 0.0f, 0.0f, (float)background.width, (float)background.height };
        Rectangle dest   = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
        Vector2 origin   = { 0.0f, 0.0f };
        Vector2 left = {1.0f, 0.0f};

        DrawTexturePro(background, source, dest, origin, 0.0f, WHITE);
        DrawTextureEx(logo, left, 0.0f, 0.7f, WHITE);

        if (DrawButton(PlayBtn, "Play", 20, LIGHTGRAY, GRAY, DARKGRAY, BLACK)) {
            currentScreen = SCREEN_GAMEPLAY;
        }

        if (DrawButton(ExitBtn, "Exit", 20, LIGHTGRAY, GRAY, DARKGRAY, BLACK)) {
            CloseWindow();
            return 0;
        }

        
        EndDrawing();
    }

    else if (currentScreen == SCREEN_GAMEPLAY) {
        
        BeginDrawing();
        ClearBackground(BLUE);

        DrawTextureEx(Hero, HeroPosition, 0.0f, 0.5f, WHITE);

        DrawRectangle(posX, posY, rectWidth, rectHeight, BLACK);

        EndDrawing();
    }
    }

    UnloadTexture(Hero);
    UnloadTexture(logo);
    UnloadTexture(background);
    CloseWindow();

    return 0;
}