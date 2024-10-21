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
#include "headers/menu.h"
#include "headers/graphic.h"
#include "headers/game.h"

pthread_mutex_t enemiesListLock;
pthread_mutex_t projectileListLock;
pthread_mutex_t playerLock;
pthread_mutex_t gameUpdateLock;
pthread_mutex_t cameraLock;

int main(int argc, char *argv[])
{
    srand(time(NULL));

    pthread_mutex_init(&enemiesListLock, NULL);
    pthread_mutex_init(&projectileListLock, NULL);
    pthread_mutex_init(&gameUpdateLock, NULL);
    pthread_mutex_init(&playerLock, NULL);
    pthread_mutex_init(&cameraLock, NULL);
    pthread_t drawingThreadId = { 0 };

    int mainError;

    States gameStatus = MENU;
    GameDataS gameData;

    gameData.toDraw = (ToDraw*)malloc(sizeof(ToDraw));
    *gameData.toDraw = MAINMENU;
    
    gameData.gameSkin = (GameSkinS*)malloc(sizeof(GameSkinS));
    (*gameData.gameSkin).primaryColor = BLUE;
    (*gameData.gameSkin).secondaryColor = WHITE;
    
    gameData.mousePosition = (Vector2*)malloc(sizeof(Vector2));
    *gameData.mousePosition = GetMousePosition();

    gameData.frameCounter = 0;
    gameData.camera = NULL;
    gameData.player = NULL;
    gameData.mapBorder = NULL;
    gameData.enemiesHead = NULL;
    gameData.projectileHead = NULL;

    mainError = pthread_create(&drawingThreadId, NULL, HandleGraphics, &gameData); 
    if (mainError != 0) TraceLog(LOG_ERROR, "Error creating thread");
    // waiting for the mouse pos to be updated
    // for the first time
    pthread_mutex_lock(&gameUpdateLock);

    while(gameStatus != EXITGAME){
        switch(gameStatus)
        {
            case MENU:
                pthread_mutex_lock(&gameUpdateLock);
                *gameData.mousePosition = GetMousePosition();
                MainMenuHandler(&gameStatus, gameData.mousePosition);
            break;
            case PLAY:
                GameHandler(&gameData);
                gameStatus = MENU;
            break;
        }
    }
    
    *gameData.toDraw = CLOSEGAME;
    mainError = pthread_join(drawingThreadId, NULL);
    if (mainError != 0) TraceLog(LOG_ERROR, "Error merging threads");
    return 0;
}