/*
    2d game, still working on its identity
    Copyright (C) 2024  blue_dabdidabdae

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.

*/

#include <ctime>
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/menu.h"
#include "headers/graphic.h"
#include "headers/game.h"
#include "headers/settings.h"

pthread_mutex_t enemiesListLock;
pthread_mutex_t projectileListLock;
pthread_mutex_t playerLock;
pthread_mutex_t gameUpdateLock;
pthread_mutex_t cameraLock;
pthread_mutex_t frameCounterLock;
pthread_mutex_t mapLock;
pthread_mutex_t weaponDataLock;

void InitData(AppDataS &appData);

int main(int argc, char *argv[])
{
    int error;
    AppDataS appData;
    States gameStatus = MENU;

    /*
    * Raylib log levels:
    * LOG_ALL: 0
    * LOG_TRACE: 1
    * LOG_DEBUG: 2
    * LOG_INFO: 3
    * LOG_WARNING: 4
    * LOG_ERROR: 5
    * LOG_FATAL: 6
    * LOG_NONE: 7
    */
    SetTraceLogLevel(LOG_ALL);

    srand(time(NULL));

    pthread_t drawingThreadId = { 0 };

    // initializing appata
    InitData(appData);

    // initializing drawing thread
    error = pthread_create(&drawingThreadId, NULL, HandleGraphics, &appData); 
    if (error)
    {
        TraceLog(LOG_ERROR, "Error creating thread");
        return 0;
    }
    appData.toDraw = DRAWMAINMENU;

    // waiting clear to update from drawing thread
    pthread_mutex_lock(&gameUpdateLock);

    // starting game loop
    while(EXITGAME != gameStatus){
        switch(gameStatus)
        {
            case MENU:
                MainMenuHandler(gameStatus, appData.toDraw);
            break;
            case SETTINGS:
                SettingsHandler(appData.toDraw, appData.settingsFlags);
                gameStatus = MENU;
            break;
            case PLAY:
                error = GameHandler(appData);
                if (error != 0)
                {
                    TraceLog(LOG_ERROR, "Game error, see prev. errors");
                    appData.toDraw = DRAWCLOSEGAME;
                    gameStatus = EXITGAME;
                }
                else gameStatus = MENU;
            break;
        }
    }

    appData.toDraw = DRAWCLOSEGAME;
    error = pthread_join(drawingThreadId, NULL);
    if (error)
    {
        // per ora la gestione del thread rimarrà così, mi basta sapere
        // se c'è stato un problema o no
        TraceLog(LOG_ERROR, "Error merging threads, trying to force kill it");
        error = pthread_cancel(drawingThreadId);
        if(error)
        {
            TraceLog(LOG_ERROR, "Couldn't kill thread, hope the OS works it out");
            TraceLog(LOG_ERROR, "<< ABORTING >>");
            abort();
        }
    }
    
    // normal game exit
    TraceLog(LOG_INFO, "Game terminated succesfully");
    return 0;
}

void InitData(AppDataS &appData)
{
    pthread_mutex_init(&enemiesListLock, NULL);
    pthread_mutex_init(&projectileListLock, NULL);
    pthread_mutex_init(&gameUpdateLock, NULL);
    pthread_mutex_init(&playerLock, NULL);
    pthread_mutex_init(&cameraLock, NULL);
    pthread_mutex_init(&weaponDataLock, NULL);
    pthread_mutex_init(&mapLock, NULL);
    pthread_mutex_init(&mapLock, NULL);
    pthread_mutex_init(&weaponDataLock, NULL);

    // the second thread isn't running yet
    // no need to use any mutex
    appData.settingsFlags.toggleFullscreen = false;
    appData.toDraw = DRAW_WAIT;
    appData.gameData = NULL;
}