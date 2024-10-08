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
#include "headers/player.h"
#include "headers/enemies.h"
#include "headers/game_engine.h"

int GameEngine()
{
    srand(time(NULL));

    TraceLog(LOG_ERROR, "DECLARING THREAD ID");
    pthread_t drawingThreadId = { 0 };

    int error;

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

    TraceLog(LOG_ERROR, "ASSIGNING THREAD DATA");
    drawArgs.camera = &camera;
    drawArgs.currentEnemy = enemiesHead;
    drawArgs.player = &player;
    drawArgs.mapBorder = mapBorder;
    drawArgs.projectileHead = projectileHead;

    while(1)
    {
        frameCounter++;
        error = pthread_create(&drawingThreadId, NULL, DrawGame, &drawArgs); 
        if (error != 0) TraceLog(LOG_ERROR, "Error creating loading thread");
        //DrawGame(&camera, enemiesHead, &player, mapBorder, projectileHead);
        
        if(IsKeyPressed(KEY_M))
        {
            //if(GameMenuHandler() == 3)
            error = pthread_join(drawingThreadId, NULL);
            if (error != 0) TraceLog(LOG_ERROR, "Error joining loading thread");
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
        error = pthread_join(drawingThreadId, NULL);
        if (error != 0) TraceLog(LOG_ERROR, "Error joining loading thread");
    }
}

void* DrawGame(void* dataPointer)
{
    DrawArgsS *data = (DrawArgsS*)dataPointer;
    TraceLog(LOG_ERROR, "UPDATING CAMERA");
    // Updating camera target to the player position
    data->camera->target = (Vector2){ data->player->x + 20, data->player->y + 20 };

    BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(*data->camera);
            TraceLog(LOG_ERROR, "DRAWING PROJECTILES");
            // drawing projectiles
            while(data->projectileHead->next != NULL)
            {
                data->projectileHead = data->projectileHead->next;
                DrawRectangleRec(data->projectileHead->projectile, data->projectileHead->color);
            }
            TraceLog(LOG_ERROR, "DRAWING MAP BORDERS");
            // drawing map borders
            for(int i = 0; i < 4; i++)
                DrawRectangleRec(data->mapBorder[i], BLUE);
            TraceLog(LOG_ERROR, "DRAWING ENEMIES");
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
            TraceLog(LOG_ERROR, "DRAWING PLAYER");
            // drawing player
            DrawRectangleRec(*data->player, RAYWHITE);
        EndMode2D();
    EndDrawing();

    pthread_exit(NULL);
}