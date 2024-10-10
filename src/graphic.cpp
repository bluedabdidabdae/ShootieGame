// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/projectiles.h"
#include "headers/enemies.h"
#include "headers/graphic.h"

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;

void *HandleGraphics(void* data)
{
    GameDataS *gameData = (GameDataS*)data;

    gameData->mousePosition = (Vector2*)malloc(sizeof(Vector2));

    InitWindow(WIDTH, HEIGT, WINDOWNAME);
    SetTargetFPS(TARGETFPS);

    *gameData->mousePosition = GetMousePosition();
    // sending clear to update to main after
    // registering mouse pos for the first time
    pthread_mutex_unlock(&gameUpdateLock);

    while(*gameData->toDraw != CLOSEGAME)
    {
        *gameData->mousePosition = GetMousePosition();
        switch(*gameData->toDraw)
        {
            case MAINMENU:
                pthread_mutex_unlock(&gameUpdateLock);
                DrawMenu();
            break;
            case GAME:
                //sending clear to update to game engine
                pthread_mutex_unlock(&gameUpdateLock);
                DrawGame(gameData);
            break;
        }
        gameData->frameCounter += 1;
        if(gameData->frameCounter >= 60)
            gameData->frameCounter = 0;
    }

    CloseWindow();
    pthread_exit(NULL);
    return NULL;
}

void DrawMenu()
{
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("Shootie Shootie Game"), MAINMENUBUTTONX, 100, 70, MAINMENUTEXTCOLOR);
    
        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Play"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+5, 40, MAINMENUTEXTCOLOR);

        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY+100, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Exit"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+105, 40, MAINMENUTEXTCOLOR);
    EndDrawing();
}

void DrawGame(void* data)
{
    GameDataS *gameData = (GameDataS*)data;
    pthread_mutex_lock(&enemiesListLock);
    EnemyLL *enemiesHead = gameData->enemiesHead;
    pthread_mutex_unlock(&enemiesListLock);
    pthread_mutex_lock(&projectileListLock);
    ProjectileLL *projectileHead = gameData->projectileHead;
    pthread_mutex_unlock(&projectileListLock);
    // Updating camera target to the player position
    
    
        ClearBackground(BLACK);
        pthread_mutex_lock(&cameraLock);
        BeginMode2D(*gameData->camera);
            // drawing player
            pthread_mutex_lock(&playerLock);
            DrawRectangleRec(*gameData->player, RAYWHITE);
            pthread_mutex_unlock(&playerLock);
            pthread_mutex_unlock(&cameraLock);
            // drawing projectiles
            pthread_mutex_lock(&projectileListLock);
            while(projectileHead->next != NULL)
            {
                projectileHead = projectileHead->next;
                DrawRectangleRec(projectileHead->projectile, projectileHead->color);
            }
            pthread_mutex_unlock(&projectileListLock);
            // drawing map borders
            for(int i = 0; i < 4; i++)
                DrawRectangleRec(gameData->mapBorder[i], BLUE);
            
            // drawing enemies from linked list of type *EnemyLL
            pthread_mutex_lock(&enemiesListLock);
            while(enemiesHead->next != NULL)
            {
                enemiesHead = enemiesHead->next;
                DrawRectangleRec(enemiesHead->healthBar, RED);
                DrawRectangle(enemiesHead->healthBar.x,
                              enemiesHead->healthBar.y,
                              enemiesHead->hitPoint,
                              enemiesHead->healthBar.height,
                              GREEN);
                DrawRectangleRec(enemiesHead->enemy, enemiesHead->color);
            }
            pthread_mutex_unlock(&enemiesListLock);
        EndMode2D();
    EndDrawing();
}
