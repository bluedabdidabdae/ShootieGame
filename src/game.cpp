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

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;
extern pthread_mutex_t frameCounterLock;
extern pthread_mutex_t mapLock;

void UpdateCameraMousePosition(GameDataS *gameData);
void CloseGame(GameDataS *gameData);
int InitGameData(GameDataS *gameData);

int GameHandler(GameDataS *gameData)
{
    int err;

    uint lastFrameId = 0;

    // gathering data from files
    err = GatherData(gameData);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error gathering game data - ABORTING");
        // ignoring CloseGame ret value;
        CloseGame(gameData);
        return err;
    }

    // loading map from files
    err = LoadMap(gameData, 0);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error loading map - ABORTING");
        // ignoring LoadMap ret value;
        CloseGame(gameData);
        return err;
    }

    // initializing the rest of the data
    err = InitGameData(gameData);
    if(err != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game memory - ABORTING");
        // ignoring CloseGame ret value
        CloseGame(gameData);
        return err;
    }

    pthread_mutex_lock(&gameUpdateLock);
    *gameData->toDraw = GAME;

    while(1)
    {
        pthread_mutex_lock(&gameUpdateLock);
                
        if(IsKeyPressed(KEY_M) || gameData->player->lives <= 0)
        {
            //if(GameMenuHandler() == 3)
            // returns nothing
            CloseGame(gameData);
            return 0;
        }
        else
        {
            pthread_mutex_lock(&cameraLock);
            pthread_mutex_lock(&playerLock);
            pthread_mutex_lock(&mapLock);

            // returns nothing
            UpdatePlayer(gameData->player, gameData->level);

            pthread_mutex_unlock(&mapLock);

            // returns nothing
            UpdateCameraMousePosition(gameData);

            pthread_mutex_unlock(&cameraLock);
            pthread_mutex_lock(&projectileListLock);
            
            pthread_mutex_lock(&frameCounterLock);
            //TraceLog(LOG_ERROR, TextFormat("Current frame: %u", lastFrameId));
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)
                && gameData->frameCounter >=
                lastFrameId + gameData->weaponsList[gameData->player->activeWeaponId].shotsDeelay)
            {
                lastFrameId = gameData->frameCounter;
                // returns nothing
                PlayerShooting(gameData);
            }
            pthread_mutex_unlock(&frameCounterLock);

            pthread_mutex_lock(&enemiesListLock);
            pthread_mutex_lock(&mapLock);

            // returns nothing
            UpdateEnemies(gameData->enemiesHead, &gameData->player->player, gameData->level);
            
            pthread_mutex_unlock(&mapLock);
            // returns nothing
            EnemiesShooting(gameData);
            // returns nothing
            UpdateProjectiles(gameData->projectileHead);
            // returns nothing
            pthread_mutex_lock(&mapLock);
            // returns nothing
            CheckProjectilesBorders(gameData->projectileHead, gameData->level);
            
            pthread_mutex_unlock(&mapLock);
            // returns nothing
            CheckProjEntityDamage(gameData);

            pthread_mutex_unlock(&projectileListLock);
            pthread_mutex_unlock(&playerLock);
            pthread_mutex_unlock(&enemiesListLock);
        }
    }
}

int InitGameData(GameDataS *gameData)
{
    gameData->score = 104;

    // Init player position (other par inited from json file)
    (*gameData->player).player.x = 950;
    (*gameData->player).player.y = 400;

    // Init enemies linked list
    gameData->enemiesHead = (EnemyLL*)malloc(sizeof(EnemyLL));
    if(gameData->enemiesHead == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated enemies head memory");
    gameData->enemiesHead->next = NULL;

    // Init projectiles linked list
    gameData->projectileHead = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    if(gameData->projectileHead == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated proejctile head memory");
    gameData->projectileHead->next = NULL;

    // Spawning 4 enemies for testing purposes
    // ignoring return values (0/-1)
    SpawnEnemy(gameData, 150, 150, NORMAL);
    SpawnEnemy(gameData, 1020, 400, NORMAL);
    SpawnEnemy(gameData, 234, 467, NORMAL);
    SpawnEnemy(gameData, 345, 340, NORMAL);

    // Setting up camera to 2d mode and centering it to the player
    gameData->camera = (Camera2D*)malloc(sizeof(Camera2D));
    if(gameData->camera == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated camera memory");
    *gameData->camera = { 0 };
    (*gameData->camera).target = (Vector2){ (*gameData->player).player.x + (*gameData->player).player.width,
                                            (*gameData->player).player.y + (*gameData->player).player.height };
    (*gameData->camera).offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    (*gameData->camera).zoom = 1.0f;

    // Temporary map borderes
    gameData->mapBorder = (Rectangle*)malloc(sizeof(Rectangle)*4);
    if(gameData->mapBorder == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated map border memory");
    gameData->mapBorder[0] = {0, 0, WIDTH, WALLTHICKNESS};
    gameData->mapBorder[1] = {0, 0, WALLTHICKNESS, HEIGT};
    gameData->mapBorder[2] = {0, HEIGT-WALLTHICKNESS, WIDTH, WALLTHICKNESS};
    gameData->mapBorder[3] = {WIDTH-WALLTHICKNESS, 0, WALLTHICKNESS, HEIGT};

    return 0;
}

void UpdateCameraMousePosition(GameDataS *gameData)
{
    if(IsKeyDown(KEY_SPACE))
    {
        // update camera position to track player and get mouse pos relative to player
        gameData->camera->target = (Vector2){ (*gameData->player).player.x + (*gameData->player).player.width,
                                              (*gameData->player).player.y + (*gameData->player).player.height };
        *gameData->mousePosition = GetMousePosition();
        // normalize mouse coordinates to new camera position
        gameData->mousePosition->x += (*gameData->player).player.x + (*gameData->player).player.width - gameData->camera->offset.x;
        gameData->mousePosition->y += (*gameData->player).player.y + (*gameData->player).player.height - gameData->camera->offset.y;
    }
    else
    {
        // update mouse position
        *gameData->mousePosition = GetMousePosition();
        // normalize mouse coordinates to new camera position
        gameData->mousePosition->x += (gameData->camera->target.x - gameData->camera->offset.x);
        gameData->mousePosition->y += (gameData->camera->target.y - gameData->camera->offset.y);
    }
}

void CloseGame(GameDataS *gameData)
{
    *gameData->toDraw = MAINMENU;
    pthread_mutex_lock(&gameUpdateLock);
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->enemiesHead != NULL)
    {
        // delete all enemies
        CompletelyDeleteAllEnemies(gameData->enemiesHead);
        gameData->enemiesHead = NULL;
        TraceLog(LOG_DEBUG, "Deallocated enemies ll");
    }
    else TraceLog(LOG_DEBUG, "Enemies ll was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->projectileHead != NULL)
    {
        // delete all projectiles
        CompletelyDeleteAllProjectiles(gameData->projectileHead);
        gameData->projectileHead = NULL;
        TraceLog(LOG_DEBUG, "Deallocated projectile ll");
    }
    else TraceLog(LOG_DEBUG, "Projectile ll was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->mapBorder != NULL)
    {
        // delete map border
        free(gameData->mapBorder);
        gameData->mapBorder = NULL;
        TraceLog(LOG_DEBUG, "Deallocated mapBorder ll");
    }
    else TraceLog(LOG_DEBUG, "Map border ll was not allocated");
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
    if(gameData->enemiesList != NULL)
    {
        // delete enemiesList
        free(gameData->enemiesList);
        gameData->enemiesList = NULL;
        TraceLog(LOG_DEBUG, "Deallocated enemiesList memory");
    }
    else TraceLog(LOG_DEBUG, "EnemiesList was not allocated");
}