// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <ctime> // FOR LINUX
// #include <time.h> // FOR WINDOWS

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/projectiles.h"
#include "headers/player.h"
#include "headers/enemies.h"
#include "headers/game_engine.h"

int GameEngine()
{
    srand(time(NULL));

    pthread_t drawingThread;

    DrawArgsS drawArgs;

    uint frameCounter = 0;
    Rectangle player = { WIDTH/2-20, HEIGT/2-20, 40, 40 };

    // Init enemies linked list
    EnemyLL *enemiesHead = (EnemyLL*)malloc(sizeof(EnemyLL));
    enemiesHead->next = NULL;

    // Init projectiles linked list
    ProjectileLL *projectileHead = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    projectileHead->next = NULL;

    // Spawning 4 enemies for testing purposes
    SpawnEnemy(enemiesHead, 40, 40);
    SpawnEnemy(enemiesHead, 20, 400);
    SpawnEnemy(enemiesHead, 234, 467);
    SpawnEnemy(enemiesHead, 345, 340);
    
    // Setting up camera to 2d mode and centering it to the player
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    camera.zoom = 0.6f;

    // Temporary map borderes
    Rectangle mapBorder[] = { {0, 0, WIDTH+WALLTHICKNESS, WALLTHICKNESS}, 
                              {0, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS},
                              {0, HEIGT, WIDTH+WALLTHICKNESS, WALLTHICKNESS},
                              {WIDTH, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS} };

    drawArgs.camera = &camera;
    drawArgs.currentEnemy = enemiesHead;
    drawArgs.player = &player;
    drawArgs.mapBorder = mapBorder;
    drawArgs.projectileHead = projectileHead;

    while(1)
    {
        frameCounter++;
        pthread_create(&drawingThread, NULL, DrawGame, &drawArgs); 
        //DrawGame(&camera, enemiesHead, &player, mapBorder, projectileHead);

        if(IsKeyPressed(KEY_M))
        {
            //if(GameMenuHandler() == 3)
            pthread_join(drawingThread, NULL);
            CompletelyDeleteAllEnemies(enemiesHead);
            CompletelyDeleteAllProjectiles(projectileHead);
            return 0;
        }
        else
        {
            UpdatePlayer(&player);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                PlayerShooting(frameCounter, projectileHead, &player);
            UpdateEnemies(enemiesHead, &player);
            SnapEnemies(enemiesHead, mapBorder);
            EnemiesShooting(enemiesHead, projectileHead, &player);
            UpdateProjectiles(projectileHead);
            CheckProjectilesBorders(projectileHead, mapBorder);
        }
        pthread_join(drawingThread, NULL);
    }
}

void* DrawGame(void* dataPointer)
{
    DrawArgsS *data = (DrawArgsS*)dataPointer;

    // Updating camera target to the player position
    data->camera->target = (Vector2){ data->player->x + 20, data->player->y + 20 };

    BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(*data->camera);
            
            // drawing projectiles
            while(data->currentEnemy->next != NULL)
            {
                DrawRectangleRec(data->projectileHead->projectile, data->projectileHead->color);
                data->projectileHead = data->projectileHead->next;
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