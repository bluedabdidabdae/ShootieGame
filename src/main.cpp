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
#include "headers/projectiles.h"
#include "headers/enemies.h"
#include "headers/main.h"
#include "headers/menu.h"
#include "headers/graphic.h"
#include "headers/game_engine.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    pthread_t drawingThreadId = { 0 };

    int mainError;

    States gameStatus;
    GameDataS gameData;

    *gameData.toDraw = MAINMENU;
    gameData.mousePosition = NULL;
    gameData.camera = NULL;
    gameData.player = NULL;
    gameData.mapBorder = NULL;
    gameData.enemiesHead = NULL;
    gameData.projectileHead = NULL;

    mainError = pthread_create(&drawingThreadId, NULL, HandleGraphics, &gameData); 
    if (mainError != 0) TraceLog(LOG_ERROR, "Error creating thread");

    do{
        MainMenuHandler(&gameStatus, drawInfo.mousePosition);
        switch(gameStatus)
        {
            case MENU: break;
            case PLAY:
                GameEngine(&drawInfo);
                *gameData.toDraw = MAINMENU;
                gameStatus = MENU;
            break;
        }
    }while(gameStatus != EXITGAME);
    
    *gameData.toDraw = CLOSEGAME;
    mainError = pthread_join(drawingThreadId, NULL);
    if (mainError != 0) TraceLog(LOG_ERROR, "Error merging threads");
    return 0;
}