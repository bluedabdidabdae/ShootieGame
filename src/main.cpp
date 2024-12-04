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

int InitData(GameDataS &gameData);
int DeleteData(GameDataS &gameData);
int ForceThreadKill(pthread_t *thread);


int main(int argc, char *argv[])
{
    int error;
    GameDataS gameData;
    States gameStatus = MENU;

    SetTraceLogLevel(LOG_ALL);

    srand(time(NULL));

    pthread_t drawingThreadId = { 0 };

    // initializing game data
    error = InitData(gameData);
    if(error != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game data");
        DeleteData(gameData);
        return 0;
    }

    // initializing drawing thread
    error = pthread_create(&drawingThreadId, NULL, HandleGraphics, &gameData); 
    if (error != 0)
    {
        TraceLog(LOG_ERROR, "Error creating thread");
        DeleteData(gameData);
        return 0;
    }

    // waiting clear to update from drawing thread
    pthread_mutex_lock(&gameUpdateLock);

    // starting game loop
    while(gameStatus != EXITGAME){
        switch(gameStatus)
        {
            case MENU:
                MainMenuHandler(&gameStatus, gameData.toDraw);
            break;
            case SETTINGS:
                SettingsHandler(gameData.toDraw, &gameData.settingsFlags);
                gameStatus = MENU;
            break;
            case PLAY:
                error = GameHandler(&gameData);
                if (error != 0)
                {
                    TraceLog(LOG_ERROR, "Game error, see prev. errors");
                    *gameData.toDraw = DRAWCLOSEGAME;
                    gameStatus = EXITGAME;
                }
                else gameStatus = MENU;
            break;
        }
    }

    *gameData.toDraw = DRAWCLOSEGAME;
    error = pthread_join(drawingThreadId, NULL);
    if (error != 0)
    {
        // per ora la gestione del thread rimarrà così, mi basta sapere
        // se c'è stato un problema o no
        TraceLog(LOG_ERROR, "Error merging threads, trying to force kill it");
        error = ForceThreadKill(&drawingThreadId);
        if(error != 0)
        {
            TraceLog(LOG_ERROR, "Situation is shit, couldn't kill thread, hope the OS works it out");
            TraceLog(LOG_ERROR, "<< ABORTING >>");
            TraceLog(LOG_ERROR, "Trying to make drawing thread to close the window");
            CloseWindow();
            DeleteData(gameData);
            TraceLog(LOG_DEBUG, "Deleted game data");
            *gameData.toDraw = DRAWABORT;
            abort();
        }
    }
    
    // normal game exit
    DeleteData(gameData);
    TraceLog(LOG_INFO, "Game terminated succesfully");
    return 0;
}

int ForceThreadKill(pthread_t *thread)
{
    int error;
    error = pthread_cancel(*thread);
    if(error != 0)
        return THREAD_ERROR;
    return 0;
}

int DeleteData(GameDataS &gameData)
{
    if(gameData.toDraw != NULL)
    {
        free(gameData.toDraw);
        TraceLog(LOG_DEBUG, "Deallocated toDraw memory");
    }
    else TraceLog(LOG_DEBUG, "ToDraw memory was not allocated");

    if(gameData.mousePosition != NULL)
    {
        free(gameData.mousePosition);
        TraceLog(LOG_DEBUG, "Deallocated mousePosition memory");
    }
    else TraceLog(LOG_DEBUG, "To mouseposition memory was not allocated");

    return 0;
}

int InitData(GameDataS &gameData)
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

    gameData.toDraw = (ToDraw*)malloc(sizeof(ToDraw));
    if(gameData.toDraw == NULL) return MALLOC_ERROR;
    
    gameData.mousePosition = (Vector2*)malloc(sizeof(Vector2));
    if(gameData.mousePosition == NULL) return MALLOC_ERROR;
    *gameData.mousePosition = GetMousePosition();

    gameData.frameCounter = 0;
    gameData.camera = NULL;
    gameData.player = NULL;
    gameData.mapTextures = NULL;
    gameData.enemiesTemplateList = NULL;
    gameData.weaponsList = NULL;
    gameData.level = NULL;

    return 0;
}