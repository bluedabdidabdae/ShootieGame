// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/menu.h"

#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270

extern pthread_mutex_t gameUpdateLock;

// local functions
States CheckMenuCollisions(Vector2* mousePosition);

void MainMenuHandler(States *gameStatus, Vector2 *mousePosition, ToDraw *toDraw)
{
    *toDraw = DRAWMAINMENU;
    do{
        pthread_mutex_lock(&gameUpdateLock);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            *mousePosition = GetMousePosition();
            *gameStatus = CheckMenuCollisions(mousePosition);
            return;
        }
    }while(MENU == *gameStatus);
}

States CheckMenuCollisions(Vector2 *mousePosition)
{
    int mouseX = mousePosition->x;
    int mouseY = mousePosition->y;
    
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+MAINMENUBUTTONHEIGT)
        return PLAY;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+100 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+100+MAINMENUBUTTONHEIGT)
        return SETTINGS;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+200 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+200+MAINMENUBUTTONHEIGT)
        return EXITGAME;

    // MENU "0"
    return MENU;
}