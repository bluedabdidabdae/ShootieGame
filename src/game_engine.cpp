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

int GameEngine(GameDataS *gameData)
{
    int error;

    uint frameCounter = 0;

    // Init player
    gameData->player = (Rectangle*)malloc(sizeof(Rectangle));
    *gameData->player = { WIDTH/2-20, HEIGT/2-20, 40, 40 };

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
    (*gameData->camera).target = (Vector2){ *gameData->player.x + 20.0f, *gameData->player.y + 20.0f };
    (*gameData->camera).offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    (*gameData->camera).zoom = 0.6f;

    // Temporary map borderes
    gameData->mapBorder = (Rectangle*)malloc(sizeof(Rectangle)*4)
    gameData->mapBorder[] = { {0, 0, WIDTH+WALLTHICKNESS, WALLTHICKNESS}, 
                              {0, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS},
                              {0, HEIGT, WIDTH+WALLTHICKNESS, WALLTHICKNESS},
                              {WIDTH, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS} };

    while(1)
    {
        frameCounter++;
        //DrawGame(&camera, enemiesHead, &player, mapBorder, projectileHead);
        
        if(IsKeyPressed(KEY_M))
        {
            //if(GameMenuHandler() == 3)
            // delete all enemies
            CompletelyDeleteAllEnemies(gameData->enemiesHead);
            gameData->enemiesHead = NULL;
            // delete all projectiles
            CompletelyDeleteAllProjectiles(gameData->projectileHead);
            gameData->projectileHead = NULL;
            // delete player
            free(gameData->player);
            gameData->player = NULL;
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
            UpdatePlayer(gameData->player);
            /*
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                PlayerShooting(frameCounter, gameData->enemiesHead, &gameData->player);
            UpdateEnemies(gameData->enemiesHead, &gameData->player);
            SnapEnemies(gameData->enemiesHead, gameData->mapBorder);
            EnemiesShooting(gameData->enemiesHead, gameData->projectileHead, &gameData->player);
            UpdateProjectiles(gameData->projectileHead);
            CheckProjectilesBorders(gameData->projectileHead, gameData->mapBorder);
            */
        }
    }
}

void* DrawGame(void* dataPointer)
{
    GameDataS *data = (GameDataS*)dataPointer;
    // Updating camera target to the player position
    data->camera->target = (Vector2){ data->player->x + 20, data->player->y + 20 };

    
        ClearBackground(BLACK);
        BeginMode2D(*data->camera);
            // drawing projectiles
            while(data->projectileHead->next != NULL)
            {
                data->projectileHead = data->projectileHead->next;
                DrawRectangleRec(data->projectileHead->projectile, data->projectileHead->color);
            }
            // drawing map borders
            for(int i = 0; i < 4; i++)
                DrawRectangleRec(data->mapBorder[i], BLUE);
            // drawing enemies from linked list of type *EnemyLL
            while(data->currentEnemy->next != NULL)
            {
                data->currentEnemy = data->currentEnemy->next;
                DrawRectangleRec(data->currentEnemy->healthBar, RED);
                DrawRectangle(data->currentEnemy->healthBar.x,
                              data->currentEnemy->healthBar.y,
                              data->currentEnemy->hitPoint,
                              data->currentEnemy->healthBar.height,
                              GREEN);
                DrawRectangleRec(data->currentEnemy->enemy, data->currentEnemy->color);
            }
            // drawing player
            DrawRectangleRec(*data->player, RAYWHITE);
        EndMode2D();
    EndDrawing();

    pthread_exit(NULL);
}