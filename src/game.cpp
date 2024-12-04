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

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;
extern pthread_mutex_t frameCounterLock;
extern pthread_mutex_t mapLock;
extern pthread_mutex_t weaponDataLock;

void UpdateCameraMousePosition(GameDataS *gameData);
void CloseGame(GameDataS *gameData);
int InitGameData(GameDataS *gameData);
void SpawnWave(WaveLL *wave);

int GameHandler(GameDataS *gameData)
{
    int err;

    uint lastFrameId = 0;

    GameStates gameStatus;

    // loading weapon, player
    // and enemies models from file
    err = GatherData(gameData);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error gathering game data - ABORTING");
        CloseGame(gameData);
        return err;
    }

    // loading level bitmap and waves from files
    err = LoadLevel(&(gameData->level), 0);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error loading map - ABORTING");
        CloseGame(gameData);
        return err;
    }

    // initializing the rest of the data
    err = InitGameData(gameData);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game memory - ABORTING");
        CloseGame(gameData);
        return err;
    }

    pthread_mutex_lock(&gameUpdateLock);
    *gameData->toDraw = DRAW_LOAD_TEXTURES;

    SpawnEnemies(5, MINION, gameData->enemiesList, gameData->enemiesTemplateList, gameData->level->bitmap);
    SpawnEnemies(5, SNIPER, gameData->enemiesList, gameData->enemiesTemplateList, gameData->level->bitmap);

    pthread_mutex_lock(&gameUpdateLock);
    *gameData->toDraw = DRAWGAME;
    gameStatus = GAME_PLAY;

    while(GAME_EXIT != gameStatus)
    {
        pthread_mutex_lock(&gameUpdateLock);

        // handling the menu while inside the game
        if(IsKeyPressed(KEY_M))
        {
            switch(SettingsHandler(gameData->toDraw, &gameData->settingsFlags))
            {
                case SETTINGS_QUIT:
                    gameStatus = GAME_EXIT;
                break;
                case SETTINGS_BACK:
                    *gameData->toDraw = DRAWGAME;
                break;
            }
            continue;
        }

        // if the player is dead quit the game
        if(gameData->player->lives <= 0)
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
        UpdatePlayer(gameData->player, gameData->level->bitmap, gameData->frameCounter);

        TraceLog(LOG_DEBUG, "Updating camera and mouse");
        UpdateCameraMousePosition(gameData);
        pthread_mutex_unlock(&cameraLock);
        pthread_mutex_unlock(&playerLock);
        pthread_mutex_unlock(&frameCounterLock);

        pthread_mutex_lock(&enemiesListLock);
        TraceLog(LOG_DEBUG, "Updating enemies");
        UpdateEnemies(gameData->enemiesList, &gameData->player->player, gameData->level->bitmap);
        pthread_mutex_unlock(&mapLock);
        pthread_mutex_unlock(&enemiesListLock);

        pthread_mutex_lock(&playerLock);
        pthread_mutex_lock(&frameCounterLock);
        pthread_mutex_lock(&weaponDataLock);
        
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && gameData->frameCounter >=
            lastFrameId + gameData->weaponsList[gameData->player->activeWeaponId].shotsDeelay)
        {
            lastFrameId = gameData->frameCounter;
            TraceLog(LOG_DEBUG, "Player shoots");
            pthread_mutex_lock(&projectileListLock);
            if(!gameData->player->flags.isStunned)
                PlayerShooting(gameData->player, gameData->weaponsList, gameData->projectileList, gameData->mousePosition);
            pthread_mutex_unlock(&projectileListLock);
        }
        pthread_mutex_unlock(&frameCounterLock);
        
        pthread_mutex_lock(&projectileListLock);
        
        TraceLog(LOG_DEBUG, "Enemies shooting");
        EnemiesShooting(
            gameData->enemiesList,
            gameData->projectileList,
            gameData->enemiesTemplateList,
            &gameData->player->player);

        pthread_mutex_unlock(&projectileListLock);
        pthread_mutex_unlock(&playerLock);
        pthread_mutex_unlock(&weaponDataLock);

        pthread_mutex_lock(&mapLock);
        pthread_mutex_lock(&projectileListLock);

        TraceLog(LOG_DEBUG, "Updating projectiles");
        UpdateProjectiles(gameData->projectileList, gameData->level->bitmap);

        pthread_mutex_unlock(&mapLock);

        pthread_mutex_lock(&enemiesListLock);
        pthread_mutex_lock(&playerLock);

        TraceLog(LOG_DEBUG, "Checking projectiles damage");
        CheckProjEntityDamage(*gameData);

        pthread_mutex_unlock(&projectileListLock);
        pthread_mutex_unlock(&enemiesListLock);
        pthread_mutex_unlock(&playerLock);
    }

    *gameData->toDraw = DRAW_UNLOAD_TEXTURES;
    pthread_mutex_lock(&gameUpdateLock);
    CloseGame(gameData);
    return 0;
}

int InitGameData(GameDataS *gameData)
{
    gameData->score = 104;

    // Init player position and flags (other par inited from json file)
    gameData->player->player.x = 170;
    gameData->player->player.y = 170;
    gameData->player->flags.isDodging = false;
    gameData->player->flags.isWalking = false;
    gameData->player->flags.canDodge = true;
    gameData->player->flags.isInvulnerable = false;
    gameData->player->flags.isStunned = false;

    // Setting up camera to 2d mode and centering it to the player
    gameData->camera = (Camera2D*)malloc(sizeof(Camera2D));
    if(gameData->camera == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated camera memory");
    *gameData->camera = { 0 };
    gameData->camera->target = (Vector2){ gameData->player->player.x + gameData->player->player.width,
                                            gameData->player->player.y + gameData->player->player.height };
    gameData->camera->offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    gameData->camera->zoom = 1.7f;

    return 0;
}

void UpdateCameraMousePosition(GameDataS *gameData)
{
    // update camera position to track player and get mouse pos relative to player
    gameData->camera->target = (Vector2){ gameData->player->player.x + gameData->player->player.width,
                                            gameData->player->player.y + gameData->player->player.height };
    *gameData->mousePosition = GetMousePosition();
    // normalize mouse coordinates to new camera position
    gameData->mousePosition->x += gameData->player->player.x + gameData->player->player.width - gameData->camera->offset.x;
    gameData->mousePosition->y += gameData->player->player.y + gameData->player->player.height - gameData->camera->offset.y;
}

void CloseGame(GameDataS *gameData)
{
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->player != NULL)
    {
        // delete player
        free(gameData->player);
        gameData->player= NULL;
        TraceLog(LOG_DEBUG, "Deallocated player memory");
    }
    else TraceLog(LOG_DEBUG, "Player was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->camera != NULL)
    {
        // delete camera
        free(gameData->camera);
        gameData->camera = NULL;
        TraceLog(LOG_DEBUG, "Deallocated camera memory");
    }
    else TraceLog(LOG_DEBUG, "Camera was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->weaponsList != NULL)
    {
        // delete weaponList
        free(gameData->weaponsList);
        gameData->weaponsList = NULL;
        TraceLog(LOG_DEBUG, "Deallocated weaponList memory");
    }
    else TraceLog(LOG_DEBUG, "WeaponList was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->enemiesTemplateList != NULL)
    {
        // delete enemiesList
        free(gameData->enemiesTemplateList);
        gameData->enemiesTemplateList = NULL;
        TraceLog(LOG_DEBUG, "Deallocated enemiesTemplateList memory");
    }
    else TraceLog(LOG_DEBUG, "EnemiesTemplateList was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->mapTextures != NULL)
    {
        // delete mapTextures
        free(gameData->mapTextures);
        gameData->mapTextures = NULL;
        TraceLog(LOG_DEBUG, "Deallocated mapTextures memory");
    }
    else TraceLog(LOG_DEBUG, "MapTextures was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->level != NULL)
    {
        // delete level
        free(gameData->level);
        gameData->level = NULL;
        TraceLog(LOG_DEBUG, "Deallocated level memory");
    }
    else TraceLog(LOG_DEBUG, "level was not allocated");
}