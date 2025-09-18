// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctime>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/projectiles.h"
#include "headers/enemies.h"
#include "headers/player.h"
#include "headers/game.h"
#include "headers/gather_data.h"
#include "headers/utility.h"
#include "headers/settings.h"

typedef enum game_states{
    GAME_PLAY,
    GAME_EXIT
}GameStates;

void UpdateCameraMousePosition(Camera2D &camera, Vector2 &mousePosition, PlayerS &player);
void CloseGame(GameDataS &ameData);
int InitGameData(GameDataS &gameData);

int GameHandler(AppDataS &appData)
{
    int err;

    uint lastFrameId = 0;

    GameStates gameStatus;

    appData.gameData = new GameDataS;
    GameDataS &gameMemory = *(appData.gameData);

    // loading weapon, player
    // and enemies models from file
    err = GatherData(gameMemory);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error gathering game data - ABORTING");
        CloseGame(gameMemory);
        return err;
    }
    TraceLog(LOG_DEBUG, "Loaded data from files");

    // loading level bitmap and waves from files
    err = LoadLevel(gameMemory.level, 0);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error loading level - ABORTING");
        CloseGame(gameMemory);
        return err;
    }
    TraceLog(LOG_DEBUG, "Loaded level");

    // initializing the rest of the data
    err = InitGameData(gameMemory);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game memory - ABORTING");
        CloseGame(gameMemory);
        return err;
    }

    pthread_mutex_lock(&gameUpdateLock);
    appData.toDraw = DRAW_LOAD_TEXTURES;

    //SpawnEnemies(5, MINION, gameMemory.enemiesList, gameMemory.enemiesTemplateList, gameMemory.level.map);
    //SpawnEnemies(5, SNIPER, gameMemory.enemiesList, gameMemory.enemiesTemplateList, gameMemory.level.map);
    //TraceLog(LOG_DEBUG, "Spawned test enemies");

    pthread_mutex_lock(&gameUpdateLock);
    appData.toDraw = DRAWGAME;
    gameStatus = GAME_PLAY;

    while(GAME_EXIT != gameStatus)
    {
        pthread_mutex_lock(&gameUpdateLock);

        // handling the menu while inside the game
        if(IsKeyPressed(KEY_M))
        {
            switch(SettingsHandler(appData.toDraw, appData.settingsFlags))
            {
                case SETTINGS_QUIT:
                    gameStatus = GAME_EXIT;
                break;
                case SETTINGS_BACK:
                    appData.toDraw = DRAWGAME;
                break;
            }
            continue;
        }

        // if the player is dead quit the game
        if(gameMemory.player.lives <= 0)
        {
            gameStatus = GAME_EXIT;
            continue;
        }

        // update the whole game logic
        pthread_mutex_lock(&cameraLock);
        pthread_mutex_lock(&playerLock);
        pthread_mutex_lock(&mapLock);
        pthread_mutex_lock(&frameCounterLock);

        TraceLog(LOG_DEBUG, "Updating player");
        UpdatePlayer(gameMemory.player,
                        gameMemory.level,
                        gameMemory.frameCounter,
                        gameMemory.mapTextures);

        TraceLog(LOG_DEBUG, "Updating camera and mouse");
        UpdateCameraMousePosition(gameMemory.camera,
                                    gameMemory.mousePosition,
                                    gameMemory.player);
        pthread_mutex_unlock(&cameraLock);
        pthread_mutex_unlock(&playerLock);
        pthread_mutex_unlock(&frameCounterLock);

        pthread_mutex_lock(&enemiesListLock);
        TraceLog(LOG_DEBUG, "Updating enemies");
        UpdateEnemies(gameMemory.enemiesList,
                        gameMemory.player.player,
                        gameMemory.level.map,
                        gameMemory.mapTextures);
        pthread_mutex_unlock(&mapLock);
        pthread_mutex_unlock(&enemiesListLock);

        pthread_mutex_lock(&playerLock);
        pthread_mutex_lock(&frameCounterLock);
        pthread_mutex_lock(&weaponDataLock);
        
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && gameMemory.frameCounter >=
            lastFrameId + gameMemory.weaponsList[gameMemory.player.activeWeaponId].shotsDeelay)
        {
            lastFrameId = gameMemory.frameCounter;
            TraceLog(LOG_DEBUG, "Player shoots");
            pthread_mutex_lock(&projectileListLock);
            if(!gameMemory.player.flags.isStunned)
                PlayerShooting(gameMemory.player,
                                gameMemory.weaponsList,
                                gameMemory.projectileList,
                                gameMemory.mousePosition);
            pthread_mutex_unlock(&projectileListLock);
        }
        pthread_mutex_unlock(&frameCounterLock);
        
        pthread_mutex_lock(&projectileListLock);
        
        TraceLog(LOG_DEBUG, "Enemies shooting");
        EnemiesShooting(
            gameMemory.enemiesList,
            gameMemory.projectileList,
            gameMemory.enemiesTemplateList,
            gameMemory.player.player);

        pthread_mutex_unlock(&projectileListLock);
        pthread_mutex_unlock(&playerLock);
        pthread_mutex_unlock(&weaponDataLock);

        pthread_mutex_lock(&mapLock);
        pthread_mutex_lock(&projectileListLock);

        TraceLog(LOG_DEBUG, "Updating projectiles");
        UpdateProjectiles(gameMemory.projectileList,
                            gameMemory.level,
                            gameMemory.mapTextures);

        pthread_mutex_unlock(&mapLock);

        pthread_mutex_lock(&enemiesListLock);
        pthread_mutex_lock(&playerLock);

        TraceLog(LOG_DEBUG, "Checking projectiles damage");
        CheckProjEntityDamage(gameMemory.projectileList,
                                gameMemory.enemiesList,
                                gameMemory.player);

        pthread_mutex_unlock(&projectileListLock);
        pthread_mutex_unlock(&enemiesListLock);
        pthread_mutex_unlock(&playerLock);
        
        if(gameMemory.enemiesList.empty() && !gameMemory.level.waveList.empty())
        {
            TraceLog(LOG_DEBUG, "Spawning new wave");
            SpawnWave(gameMemory.level.waveList.begin()->enemyList,
                        gameMemory.enemiesList,
                        gameMemory.enemiesTemplateList,
                        gameMemory.level.map,
                        gameMemory.mapTextures);
            gameMemory.level.waveList.pop_front();
        }
        
    }

    appData.toDraw = DRAW_UNLOAD_TEXTURES;
    pthread_mutex_lock(&gameUpdateLock);
    CloseGame(gameMemory);
    return 0;
}

int InitGameData(GameDataS &gameMemory)
{
    gameMemory.score = 104;

    // Init player position and flags (other par inited from json file)
    gameMemory.player.player.x = 200;
    gameMemory.player.player.y = 200;
    gameMemory.player.flags.isDodging = false;
    gameMemory.player.flags.isWalking = false;
    gameMemory.player.flags.canDodge = true;
    gameMemory.player.flags.isInvulnerable = false;
    gameMemory.player.flags.isStunned = false;

    // Setting up camera to 2d mode and centering it to the player
    TraceLog(LOG_DEBUG, "Initializing camera");
    gameMemory.camera = { 0 };
    TraceLog(LOG_DEBUG, "Camera initialized to 0");
    gameMemory.camera.target = (Vector2){ gameMemory.player.player.x + gameMemory.player.player.width,
                                          gameMemory.player.player.y + gameMemory.player.player.height };
    TraceLog(LOG_DEBUG, "Camera initial target defined");
    gameMemory.camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    TraceLog(LOG_DEBUG, "Camera offset defined");
    gameMemory.camera.zoom = 1.7f;
    TraceLog(LOG_DEBUG, "Camera zoom defined");

    return 0;
}

void UpdateCameraMousePosition(Camera2D &camera, Vector2 &mousePosition, PlayerS &player)
{
    // update camera position to track player and get mouse pos relative to player
    camera.target = (Vector2){ player.player.x + player.player.width,
                                          player.player.y + player.player.height };
    mousePosition = GetMousePosition();
    // normalize mouse coordinates to new camera position
    mousePosition.x += player.player.x + player.player.width - camera.offset.x;
    mousePosition.y += player.player.y + player.player.height - camera.offset.y;
}

void CloseGame(GameDataS &gameData)
{
    if(gameData.weaponsList != NULL)
    {
        // delete weaponList
        free(gameData.weaponsList);
        gameData.weaponsList = NULL;
        TraceLog(LOG_DEBUG, "Deallocated weaponList memory");
    }
    else TraceLog(LOG_DEBUG, "WeaponList was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData.enemiesTemplateList != NULL)
    {
        // delete enemiesTemplateList
        free(gameData.enemiesTemplateList);
        gameData.enemiesTemplateList = NULL;
        TraceLog(LOG_DEBUG, "Deallocated enemiesTemplateList memory");
    }
    else TraceLog(LOG_DEBUG, "EnemiesTemplateList was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    while(!gameData.level.waveList.empty())
    {
        if(!gameData.level.waveList.begin()->enemyList.empty())
            gameData.level.waveList.begin()->enemyList.clear();
        gameData.level.waveList.pop_front();
    }
    TraceLog(LOG_DEBUG, "Waves cleared from memory");
/////////////////////////////////////////////////////////////////////////////////
    free(&gameData);
}