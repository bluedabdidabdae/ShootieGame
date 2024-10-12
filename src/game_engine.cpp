// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctime>

#include "raylib.h"
#include "headers/projectiles.h"
#include "headers/enemies.h"
#include "headers/player.h"
#include "headers/graphic.h"
#include "headers/game_engine.h"

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;

int GameEngine(GameDataS *gameData)
{
    int error;

    uint lastFrameId;

    gameData->score = 104;

    // Init player
    gameData->player = (PlayerS*)malloc(sizeof(PlayerS));
    (*gameData->player).player = { WIDTH/2-20, HEIGT/2-20, 40, 40 };
    (*gameData->player).lives = 500;

    // Init enemies linked list
    gameData->enemiesHead = (EnemyLL*)malloc(sizeof(EnemyLL));
    gameData->enemiesHead->next = NULL;

    // Init projectiles linked list
    gameData->projectileHead = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    gameData->projectileHead->next = NULL;

    // Spawning 4 enemies for testing purposes
    SpawnEnemy(gameData->enemiesHead, 40, 40);
    SpawnEnemy(gameData->enemiesHead, 20, 400);
    SpawnEnemy(gameData->enemiesHead, 234, 467);
    SpawnEnemy(gameData->enemiesHead, 345, 340);
    
    // Setting up camera to 2d mode and centering it to the player
    gameData->camera = (Camera2D*)malloc(sizeof(Camera2D));
    *gameData->camera = { 0 };
    (*gameData->camera).target = (Vector2){ (*gameData->player).player.x + 20.0f, (*gameData->player).player.y + 20.0f };
    (*gameData->camera).offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    (*gameData->camera).zoom = 0.6f;

    // Temporary map borderes
    gameData->mapBorder = (Rectangle*)malloc(sizeof(Rectangle)*4);
    gameData->mapBorder[0] = {0, 0, WIDTH+WALLTHICKNESS, WALLTHICKNESS};
    gameData->mapBorder[1] = {0, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS};
    gameData->mapBorder[2] = {0, HEIGT, WIDTH+WALLTHICKNESS, WALLTHICKNESS};
    gameData->mapBorder[3] = {WIDTH, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS};

    pthread_mutex_lock(&gameUpdateLock);
    *gameData->toDraw = GAME;
    while(1)
    {
        pthread_mutex_lock(&gameUpdateLock);
        lastFrameId = gameData->frameCounter;
        //DrawGame(&camera, enemiesHead, &player, mapBorder, projectileHead);
        
        if(IsKeyPressed(KEY_M) || (*gameData->player).lives <= 0)
        {
            //if(GameMenuHandler() == 3)
            *gameData->toDraw = MAINMENU;
            pthread_mutex_lock(&gameUpdateLock);
            // delete all enemies
            CompletelyDeleteAllEnemies(gameData->enemiesHead);
            gameData->enemiesHead = NULL;
            // delete all projectiles
            CompletelyDeleteAllProjectiles(gameData->projectileHead);
            gameData->projectileHead = NULL;
            // delete player
            free(gameData->player);
            gameData->player= NULL;
            // delete camera
            free(gameData->camera);
            gameData->camera = NULL;
            // delete map border
            free(gameData->mapBorder);
            gameData->mapBorder = NULL;
            return 0;
        }
        else
        {
            pthread_mutex_lock(&cameraLock);
            pthread_mutex_lock(&playerLock);

            UpdatePlayer(&(*gameData->player).player);
            gameData->camera->target = (Vector2){ (*gameData->player).player.x + 20, (*gameData->player).player.y + 20 };
            
            pthread_mutex_unlock(&cameraLock);
            
            pthread_mutex_lock(&projectileListLock);
            
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                PlayerShooting(gameData->frameCounter, gameData->projectileHead, &(*gameData->player).player, gameData->mousePosition);

            pthread_mutex_lock(&enemiesListLock);

            UpdateEnemies(gameData->enemiesHead, &(*gameData->player).player);
            SnapEnemies(gameData->enemiesHead, gameData->mapBorder);
            EnemiesShooting(gameData->enemiesHead, gameData->projectileHead, &(*gameData->player).player);
            
            UpdateProjectiles(gameData->projectileHead);
            CheckProjectilesBorders(gameData->projectileHead, gameData->mapBorder);

            CheckProjEntityDamage(gameData);

            pthread_mutex_unlock(&projectileListLock);
            pthread_mutex_unlock(&playerLock);
            pthread_mutex_unlock(&enemiesListLock);
        }
    }
}