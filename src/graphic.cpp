// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <pthread.h>

#include "raylib.h"
#include "headers/graphic.h"

void *HandleGraphics(void* data)
{
    GameDataS gameData = *(GameDataS*)data;

    InitWindow(WIDTH, HEIGT, WINDOWNAME);
    SetTargetFPS(TARGETFPS);

    while(*gameData.toDraw != CLOSEGAME)
    {
        *gameData.mousePosition = GetMousePosition();
        switch(*gameData.toDraw)
        {
            case MAINMENU:
                DrawMenu();
            break;
        }
    }

    CloseWindow();
    pthread_exit(NULL);
    return NULL;
}

void DrawMenu()
{
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("Shootie Shootie Game"), MAINMENUBUTTONX, 100, 70, MAINMENUTEXTCOLOR);
    
        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Play"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+5, 40, MAINMENUTEXTCOLOR);

        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY+100, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Exit"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+105, 40, MAINMENUTEXTCOLOR);
    EndDrawing();
}