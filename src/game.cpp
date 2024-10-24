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

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;

void UpdateCameraMousePosition(GameDataS *gameData);
void CloseGame(GameDataS *gameData);
int InitGameData(GameDataS *gameData);

int GameHandler(GameDataS *gameData)
{
    int error;

    uint lastFrameId;

    error = InitGameData(gameData);
    if(error != 0)
    {
        TraceLog(LOG_ERROR, "Error allocating game memory - ABORTING");
        // ignoring CloseGame(...); return value
        CloseGame(gameData);
        return error;
    }

    pthread_mutex_lock(&gameUpdateLock);
    *gameData->toDraw = GAME;

    while(1)
    {
        pthread_mutex_lock(&gameUpdateLock);
        lastFrameId = gameData->frameCounter;
                
        if(IsKeyPressed(KEY_M) || (*gameData->player).lives <= 0)
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

            // returns nothing
            UpdatePlayer(&(*gameData->player).player);

            // returns nothing
            UpdateCameraMousePosition(gameData);

            pthread_mutex_unlock(&cameraLock);
            pthread_mutex_lock(&projectileListLock);
            
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // returns nothing
                PlayerShooting(gameData->frameCounter, gameData->projectileHead, &(*gameData->player).player, gameData->mousePosition);
            }

            pthread_mutex_lock(&enemiesListLock);

            // returns nothing
            UpdateEnemies(gameData->enemiesHead, &(*gameData->player).player);
            // returns nothing
            SnapEnemies(gameData->enemiesHead, gameData->mapBorder);
            // returns nothing
            EnemiesShooting(gameData->enemiesHead, gameData->projectileHead, &(*gameData->player).player);
            // returns nothing
            UpdateProjectiles(gameData->projectileHead);
            // returns nothing
            CheckProjectilesBorders(gameData->projectileHead, gameData->mapBorder);
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

    // Init player
    gameData->player = (PlayerS*)malloc(sizeof(PlayerS));
    if(gameData->player == NULL) return MALLOC_ERROR;
    TraceLog(LOG_DEBUG, "Allocated player memory");
    (*gameData->player).player = { WIDTH/2-20, HEIGT/2-20, 20, 20 };
    (*gameData->player).lives = 200;

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
    SpawnEnemy(gameData->enemiesHead, 40, 40);
    SpawnEnemy(gameData->enemiesHead, 1020, 400);
    SpawnEnemy(gameData->enemiesHead, 234, 467);
    SpawnEnemy(gameData->enemiesHead, 345, 340);

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
    }
    else TraceLog(LOG_DEBUG, "Projectile ll was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->mapBorder != NULL)
    {
        // delete map border
        free(gameData->mapBorder);
        gameData->mapBorder = NULL;
    }
    else TraceLog(LOG_DEBUG, "Map border ll was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->player != NULL)
    {
        // delete player
        free(gameData->player);
        gameData->player= NULL;
    }
    else TraceLog(LOG_DEBUG, "Player was not allocated");
/////////////////////////////////////////////////////////////////////////////////
    if(gameData->camera != NULL)
    {
        // delete camera
        free(gameData->camera);
        gameData->camera = NULL;
    }
    else TraceLog(LOG_DEBUG, "Camera was not allocated");
}