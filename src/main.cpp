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

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ctime>
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/menu.h"
#include "headers/graphic.h"
#include "headers/game.h"

pthread_mutex_t enemiesListLock;
pthread_mutex_t projectileListLock;
pthread_mutex_t playerLock;
pthread_mutex_t gameUpdateLock;
pthread_mutex_t cameraLock;
pthread_mutex_t frameCounterLock;
pthread_mutex_t weaponsLock;

int InitData(GameDataS *gameData);
int DeleteData(GameDataS *gameData);
int ForceThreadKill(pthread_t *thread);

int main(int argc, char *argv[])
{
    SetTraceLogLevel(LOG_DEBUG);

    srand(time(NULL));

    int error;

    States gameStatus = MENU;
    GameDataS gameData;

    pthread_t drawingThreadId = { 0 };

    // initializing game data
    error = InitData(&gameData);
    if(error != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game data");
        DeleteData(&gameData);
        return 0;
    }

    // initializing drawing thread
    error = pthread_create(&drawingThreadId, NULL, HandleGraphics, &gameData); 
    if (error != 0)
    {
        TraceLog(LOG_ERROR, "Error creating thread");
        DeleteData(&gameData);
        return 0;
    }

    // waiting clear to update from drawing thread
    pthread_mutex_lock(&gameUpdateLock);

    // starting game loop
    while(gameStatus != EXITGAME){
        switch(gameStatus)
        {
            case MENU:
                pthread_mutex_lock(&gameUpdateLock);
                *gameData.mousePosition = GetMousePosition();
                MainMenuHandler(&gameStatus, gameData.mousePosition);
            break;
            case PLAY:
                error = GameHandler(&gameData);
                if (error != 0)
                {
                    TraceLog(LOG_ERROR, "Game error, see prev. errors");
                    *gameData.toDraw = CLOSEGAME;
                    gameStatus = EXITGAME;
                }
                else gameStatus = MENU;
            break;
        }
    }

    *gameData.toDraw = CLOSEGAME;
    error = pthread_join(drawingThreadId, NULL);
    if (error != 0)
    {
        TraceLog(LOG_ERROR, "Error merging threads, trying to force kill it");
        error = ForceThreadKill(&drawingThreadId);
        if(error != 0)
        {
            TraceLog(LOG_ERROR, "Situation is shit, couldn't kill thread, hope the OS works it out");
            TraceLog(LOG_ERROR, "<< ABORTING >>");
            TraceLog(LOG_DEBUG, "Trying to make drawing thread to close the window");
            CloseWindow();
            DeleteData(&gameData);
            TraceLog(LOG_DEBUG, "Deleted game data");
            *gameData.toDraw = ABORT;
            abort();
        }
    }
    
    // normal game exit
    DeleteData(&gameData);
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

int DeleteData(GameDataS *gameData)
{
    if(gameData->toDraw != NULL)
    {
        free(gameData->toDraw);
        TraceLog(LOG_DEBUG, "Deallocated to draw memory");
    }
    else TraceLog(LOG_DEBUG, "To draw memory was not allocated");

    if(gameData->gameSkin != NULL)
    {
        free(gameData->gameSkin);
        TraceLog(LOG_DEBUG, "Deallocated to game skin memory");
    }
    else TraceLog(LOG_DEBUG, "Game skin memory was not allocated");

    if(gameData->mousePosition != NULL)
    {
        free(gameData->mousePosition);
        TraceLog(LOG_DEBUG, "Deallocated to mouse position memory");
    }
    else TraceLog(LOG_DEBUG, "To mouse position memory was not allocated");

    return 0;
}

int InitData(GameDataS *gameData)
{
    pthread_mutex_init(&enemiesListLock, NULL);
    pthread_mutex_init(&projectileListLock, NULL);
    pthread_mutex_init(&gameUpdateLock, NULL);
    pthread_mutex_init(&playerLock, NULL);
    pthread_mutex_init(&cameraLock, NULL);
    pthread_mutex_init(&weaponsLock, NULL);

    gameData->toDraw = (ToDraw*)malloc(sizeof(ToDraw));
    if(gameData->toDraw == NULL) return MALLOC_ERROR;
    *gameData->toDraw = MAINMENU;
    
    gameData->gameSkin = (GameSkinS*)malloc(sizeof(GameSkinS));
    if(gameData->gameSkin == NULL) return MALLOC_ERROR;
    (*gameData->gameSkin).primaryColor = BLUE;
    (*gameData->gameSkin).secondaryColor = WHITE;
    
    gameData->mousePosition = (Vector2*)malloc(sizeof(Vector2));
    if(gameData->mousePosition == NULL) return MALLOC_ERROR;
    *gameData->mousePosition = GetMousePosition();

    gameData->frameCounter = 0;
    gameData->camera = NULL;
    gameData->player = NULL;
    gameData->mapBorder = NULL;
    gameData->enemiesHead = NULL;
    gameData->projectileHead = NULL;

    return 0;
}