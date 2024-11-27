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

typedef enum settings_status{
    TOGGLEFULLSCREEN,
    EXITSETTINGS,
    NONE
}SettingsStatus;

extern pthread_mutex_t gameUpdateLock;

// local functions
SettingsStatus CheckSettingsCollisions(Vector2* mousePosition);

void SettingsHandler(States *gameStatus, Vector2 *mousePosition, ToDraw *toDraw, SettingsFlags *settingsFlags)
{
    *toDraw = DRAWSETTINGS;
    SettingsStatus settingsStatus;
    do{
        pthread_mutex_lock(&gameUpdateLock);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            *mousePosition = GetMousePosition();
            
            settingsStatus = CheckSettingsCollisions(mousePosition);
            switch(settingsStatus)
            {
                case TOGGLEFULLSCREEN:
                    settingsFlags->toggleFullscreen = true;
                break;
            }
        }
    }while(EXITSETTINGS != settingsStatus);
}

SettingsStatus CheckSettingsCollisions(Vector2 *mousePosition)
{
    int mouseX = mousePosition->x;
    int mouseY = mousePosition->y;
    
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+MAINMENUBUTTONHEIGT)
        return TOGGLEFULLSCREEN;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+100 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+100+MAINMENUBUTTONHEIGT)
        return NONE;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+200 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+200+MAINMENUBUTTONHEIGT)
        return EXITSETTINGS;

    // MENU "0"
    return NONE;
}