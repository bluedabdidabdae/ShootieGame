// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include "raylib.h"
#include "headers/global_types.h"
#include "headers/menu.h"

void MainMenuHandler(States *gameStatus)
{
    do{
        DrawMenu();
        if(WindowShouldClose())
        {
            *gameStatus = EXITGAME;
            return;
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            *gameStatus = (States)CheckCollisions();
    }while(*gameStatus == MENU);
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

States CheckCollisions()
{
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    
    // god also forgive me for this shit
    // play button returns PLAY "1"
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+MAINMENUBUTTONHEIGT) return PLAY;
    //exit button returns EXITGAME "3"
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+100 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+100+MAINMENUBUTTONHEIGT) return EXITGAME;

    // MENU "0"
    return MENU;
}