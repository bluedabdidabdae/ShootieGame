// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/graphic.h"

#define WINDOWNAME "Shootie Shootie Game"
#define WIDTH 1335
#define HEIGT 675
#define TARGETFPS 60
#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define FADEVALUE 0.2
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270
#define MAINMENUTEXTCOLOR RAYWHITE
#define WALLTHICKNESS 10

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;

// local functions
void DrawMenu();
void DrawGame(void* data);

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
                gameData->mousePosition->x += gameData->camera->target.x - (WIDTH / 2);
                gameData->mousePosition->y += gameData->camera->target.y - (HEIGT / 2);
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
            
            DrawRectangleRec((*gameData->player).player, (*gameData->gameSkin).primaryColor);

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
                DrawRectangleRec(gameData->mapBorder[i], (*gameData->gameSkin).secondaryColor);
            
            // drawing enemies from linked list of type *EnemyLL
            pthread_mutex_lock(&enemiesListLock);
            while(enemiesHead->next != NULL)
            {
                enemiesHead = enemiesHead->next;
                //DrawRectangleRec(enemiesHead->healthBar, RED);
                DrawRectangle(enemiesHead->healthBar.x,
                              enemiesHead->healthBar.y,
                              enemiesHead->hitPoint,
                              enemiesHead->healthBar.height,
                              GREEN);
                DrawRectangleRec(enemiesHead->enemy, enemiesHead->color);
            }
            pthread_mutex_unlock(&enemiesListLock);
        EndMode2D();
        pthread_mutex_lock(&playerLock);
        DrawText(TextFormat("SCORE: %u", gameData->score), 30, 30, 40, WHITE);
        DrawRectangle(30, 80, (*gameData->player).lives, 15, GREEN);
        pthread_mutex_unlock(&playerLock);
    EndDrawing();
}
