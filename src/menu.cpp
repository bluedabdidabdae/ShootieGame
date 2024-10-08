// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include "raylib.h"
#include "headers/global_types.h"
#include "headers/menu.h"

void MainMenuHandler(States *gameStatus, Vector2 mousePosition)
{
    while(*gameStatus == MENU){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            *gameStatus = CheckCollisions(&mousePosition);
            return;
        }
    }
}

States CheckCollisions(Vector2 *mousePosition)
{
    int mouseX = mousePosition->x;
    int mouseY = mousePosition->y;
    
    // play button returns PLAY "1"
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+MAINMENUBUTTONHEIGT) return PLAY;
    //exit button returns EXITGAME "3"
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+100 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+100+MAINMENUBUTTONHEIGT) return EXITGAME;

    // MENU "0"
    return MENU;
}