#include "raylib.h"
#include "../Include/tween.h"
#include "../Include/structs.h"
#include "../Include/database.h"
#include "../Include/deck.h"
#include "../Include/combat.h"
#include "../Include/map.h"
#include <time.h>

#define MAX_DATABASE_CARDS 500 // new safety limit for our card array CUZ CYAN HUMOROUSLY SAID 500

bool DrawClearButton(Rectangle, const char*, int, Color, Color);
bool DrawButton(Rectangle, const char*, int, Color, Color, Color, Color);
static void DrawCenteredText(Rectangle, const char*, int, Color);

// this is the main function that initializes the window and loads the card database
int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "SSR++ Deckbuilder");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));

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
    Texture2D DeckBG = LoadTexture("Assets/Sprites/ui elements/DeckBG.png");
    Texture2D Hero = LoadTexture("Assets/Sprites/placeholder assets/p_idl_kurokami.png");
    Texture2D forestFloor = LoadTexture("Assets/Sprites/bg elements/bg_forest1_floor.png");
    Texture2D forestBackground = LoadTexture("Assets/Sprites/bg elements/bg_forest1.png");

    const float heroScale = 0.5f;
    const int heroBottomY = (int)(HeroPosition.y + Hero.height * heroScale);
    const int forestFloorVisibleTop = forestFloor.height - 16;
    const Vector2 forestFloorPosition = {0.0f, (float)(heroBottomY - forestFloorVisibleTop - 2)};
    const Vector2 forestBackgroundPosition = {0.0f, (float)(heroBottomY - forestBackground.height)};
    posY = heroBottomY;
    rectHeight = screenHeight - posY;

    MapStep areaMap[MAP_STEP_COUNT];
    generateAreaMap(areaMap, MAP_STEP_COUNT, 1);
    int mapProgression = 1;
    int mapLane = -1;
    MapTileType selectedMapTile = MAP_TILE_START;

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

        if (DrawClearButton(PlayBtn, "Play", 40, WHITE, LIGHTGRAY)) {
            currentScreen = SCREEN_MAP;
        }

        if (DrawClearButton(ExitBtn, "Exit", 40, WHITE, LIGHTGRAY)) {
            CloseWindow();
            return 0;
        }

        
        EndDrawing();
    }

    else if (currentScreen == SCREEN_MAP) {
        
        BeginDrawing();
        ClearBackground(BLANK);

        DrawRectangle(posX, posY, rectWidth, rectHeight, DARKBLUE);
        DrawTextureEx(forestBackground, forestBackgroundPosition, 0.0f, 1.0f, WHITE);
        DrawTextureEx(forestFloor, forestFloorPosition, 0.0f, 1.0f, WHITE);
        DrawTextureEx(Hero, HeroPosition, 0.0f, heroScale, WHITE);

        const int panelCount = MAP_STEP_COUNT;
        const int panelWidth = 50;
        const int panelHeight = 160;
        const int panelGap = 20;
        const int panelRowWidth = panelCount * panelWidth + (panelCount - 1) * panelGap;
        const int panelStride = panelWidth + panelGap;
        const int panelStartX = posX + (rectWidth - panelRowWidth) / 2;
        const int panelY = posY + (rectHeight - panelHeight) / 2;
        const int squareStackY = panelY + (panelHeight - 2 * panelWidth - panelGap) / 2;
        const Vector2 mousePosition = GetMousePosition();

        for (int i = 0; i < panelCount; i++) {
            const int panelX = panelStartX + i * panelStride;

            if (mapStepHasBranches(i)) {
                Rectangle topBounds = {(float)panelX, (float)squareStackY,
                                       (float)panelWidth, (float)panelWidth};
                Rectangle bottomBounds = {(float)panelX,
                                          (float)(squareStackY + panelWidth + panelGap),
                                          (float)panelWidth, (float)panelWidth};
                bool topSelectable = i == mapProgression && (mapLane == -1 || mapLane == 0);
                bool bottomSelectable = i == mapProgression && (mapLane == -1 || mapLane == 1);

                DrawRectangleRec(topBounds, WHITE);
                DrawRectangleLinesEx(topBounds, 2.0f, topSelectable ? GOLD : DARKGRAY);
                DrawCenteredText(topBounds, mapTileLabel(areaMap[i].top), 8, BLACK);

                DrawRectangleRec(bottomBounds, WHITE);
                DrawRectangleLinesEx(bottomBounds, 2.0f, bottomSelectable ? GOLD : DARKGRAY);
                DrawCenteredText(bottomBounds, mapTileLabel(areaMap[i].bottom), 8, BLACK);

                if (topSelectable && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mousePosition, topBounds)) {
                    mapLane = 0;
                    selectedMapTile = areaMap[i].top;
                    currentScreen = mapTileScreen(selectedMapTile);
                } else if (bottomSelectable && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                           CheckCollisionPointRec(mousePosition, bottomBounds)) {
                    mapLane = 1;
                    selectedMapTile = areaMap[i].bottom;
                    currentScreen = mapTileScreen(selectedMapTile);
                }
            } else {
                Rectangle panelBounds = {(float)panelX, (float)panelY,
                                         (float)panelWidth, (float)panelHeight};
                const bool selectable = i == mapProgression;

                DrawRectangleRec(panelBounds, WHITE);
                DrawRectangleLinesEx(panelBounds, 2.0f, selectable ? GOLD : DARKGRAY);
                DrawCenteredText(panelBounds, mapTileLabel(areaMap[i].top), 9, BLACK);

                if (selectable && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mousePosition, panelBounds)) {
                    selectedMapTile = areaMap[i].top;
                    currentScreen = mapTileScreen(selectedMapTile);
                }
            }
        }

        if (mapProgression >= panelCount) {
            DrawCenteredText((Rectangle){0.0f, 20.0f, (float)screenWidth, 40.0f},
                             "AREA COMPLETE", 28, GREEN);
        }

        EndDrawing();
    }

    else if (currentScreen == SCREEN_GAMEPLAY || currentScreen == SCREEN_BATTLE ||
             currentScreen == SCREEN_SHOP || currentScreen == SCREEN_CAMP ||
             currentScreen == SCREEN_EVENT) {
        BeginDrawing();
        ClearBackground(DARKBLUE);

        DrawCenteredText((Rectangle){0.0f, 220.0f, (float)screenWidth, 60.0f},
                         mapTileLabel(selectedMapTile), 40, WHITE);

        Rectangle continueButton = {screenWidth / 2.0f - 120.0f,
                                    screenHeight - 110.0f, 240.0f, 64.0f};
        if (DrawButton(continueButton, "Continue", 28, DARKGRAY, GRAY, LIGHTGRAY, WHITE)) {
            const int previousProgression = mapProgression;
            const int nextProgression = selectNextTile(areaMap, mapProgression, MAP_STEP_COUNT);
            if (nextProgression >= 0) {
                mapProgression = nextProgression;
            }

            if (!mapStepHasBranches(previousProgression) ||
                !mapStepHasBranches(mapProgression)) {
                mapLane = -1;
            }

            currentScreen = SCREEN_MAP;
        }

        EndDrawing();
    }
    
    
    
    }

    UnloadTexture(Hero);
    UnloadTexture(forestBackground);
    UnloadTexture(forestFloor);
    UnloadTexture(logo);
    UnloadTexture(DeckBG);
    UnloadTexture(background);
    CloseWindow();

    return 0;
}

static void DrawCenteredText(Rectangle bounds, const char *text, int fontSize, Color color)
{
    const int textWidth = MeasureText(text, fontSize);
    const int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
    const int textY = (int)(bounds.y + (bounds.height - fontSize) / 2);
    DrawText(text, textX, textY, fontSize, color);
}

bool DrawClearButton(Rectangle bounds, const char *text, int fontSize, Color textColor, Color pressColor)
{
    const bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    int currentFontSize = fontSize;
    Color currentTextColor = textColor;

    if (isHovered) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            currentFontSize += 10;
            currentTextColor = pressColor;
        } else {
            currentFontSize += 15;
        }
    }

    const int textWidth = MeasureText(text, currentFontSize);
    const int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
    const int textY = (int)(bounds.y + (bounds.height - currentFontSize) / 2);
    DrawText(text, textX, textY, currentFontSize, currentTextColor);

    return isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

bool DrawButton(Rectangle bounds, const char *text, int fontSize, Color baseColor, Color hoverColor, Color pressColor, Color textColor)
{
    const bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    Color currentColor = baseColor;

    if (isHovered) {
        currentColor = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? pressColor : hoverColor;
    }

    DrawRectangleRec(bounds, currentColor);
    DrawRectangleLinesEx(bounds, 2.0f, WHITE);

    const int textWidth = MeasureText(text, fontSize);
    const int textX = (int)(bounds.x + (bounds.width - textWidth) / 2);
    const int textY = (int)(bounds.y + (bounds.height - fontSize) / 2);
    DrawText(text, textX, textY, fontSize, textColor);

    return isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}