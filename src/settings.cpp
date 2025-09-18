// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/settings.h"

#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270

// local functions
SettingsStatus CheckSettingsCollisions(Vector2 &mousePosition);

SettingsStatus SettingsHandler(ToDraw &toDraw, SettingsFlags &settingsFlags)
{
    Vector2 mousePosition;
    SettingsStatus settingsStatus;
    toDraw = DRAWSETTINGS;

    do{
        pthread_mutex_lock(&gameUpdateLock);
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            mousePosition = GetMousePosition();
            
            settingsStatus = CheckSettingsCollisions(mousePosition);
            switch(settingsStatus)
            {
                case SETTINGS_TOGGLEFULLSCREEN:
                    settingsFlags.toggleFullscreen = true;
                break;
            }
        }
    }while(SETTINGS_BACK != settingsStatus && SETTINGS_QUIT != settingsStatus);

    return settingsStatus;
}

SettingsStatus CheckSettingsCollisions(Vector2 &mousePosition)
{
    int mouseX = mousePosition.x;
    int mouseY = mousePosition.y;
    
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+MAINMENUBUTTONHEIGT)
        return SETTINGS_TOGGLEFULLSCREEN;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+100 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+100+MAINMENUBUTTONHEIGT)
        return SETTINGS_BACK;
    if(mouseX > MAINMENUBUTTONX && mouseY > MAINMENUBUTTONY+200 && mouseX < MAINMENUBUTTONX+MAINMENUBUTTONWIDTH && mouseY < MAINMENUBUTTONY+200+MAINMENUBUTTONHEIGT)
        return SETTINGS_QUIT;

    return SETTINGS_NONE;
}