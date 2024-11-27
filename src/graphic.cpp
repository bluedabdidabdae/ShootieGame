// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/graphic.h"
#include "headers/gather_data.h"

#define WINDOWNAME "Shootie Shootie Game"
#define TARGETFPS 60
#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define FADEVALUE 0.2
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270
#define MAINMENUTEXTCOLOR RAYWHITE

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;
extern pthread_mutex_t frameCounterLock;
extern pthread_mutex_t mapLock;
extern pthread_mutex_t weaponDataLock;

// local functions
void DrawMenu();
void DrawGame(GameDataS *gameData);
void LoadGameTextures(GameDataS *gameData);
void DrawMenuLoop();
void DrawGameLoop(GameDataS *gameData);
void UnloadGameTextures(GameDataS *gameData);

void *HandleGraphics(void* data)
{
    int ret = 0;
    GameDataS *gameData = (GameDataS*)data;

    InitWindow(WIDTH, HEIGT, WINDOWNAME);
    SetTargetFPS(TARGETFPS);

    // sending clear to update to main
    pthread_mutex_unlock(&gameUpdateLock);

    while(*gameData->toDraw != CLOSEGAME)
    {
        switch(*gameData->toDraw)
        {
            case MAINMENU:
                DrawMenuLoop();
            break;
            case GAME:
                LoadGameTextures(gameData);
                DrawGameLoop(gameData);
                /*
                 * its safe to interact with game data from this point
                 * since the main thread is waiting for a clear to update
                 * to deallocate all the memory relative to the game
                */
                UnloadGameTextures(gameData);
            break;
            case SETTINGS:
                
            break;
            case ABORT:
                TraceLog(LOG_DEBUG, "<< Aborting on drawing thread >>");
                abort();
            break;
        }
    }

    CloseWindow();
    pthread_exit(NULL);
    return NULL;
}

void DrawMenuLoop()
{
    pthread_mutex_unlock(&gameUpdateLock);
    DrawMenu();
}

void DrawGameLoop(GameDataS *gameData)
{
    while(GAME == *gameData->toDraw)
    {
        //sending clear to update to game engine
        pthread_mutex_unlock(&gameUpdateLock);
        DrawGame(gameData);

        pthread_mutex_lock(&frameCounterLock);
        TraceLog(LOG_DEBUG, "Updating frame counter");
        gameData->frameCounter += 1;
        pthread_mutex_unlock(&frameCounterLock);
    }
}

void UnloadGameTextures(GameDataS *gameData)
{
    int i;

    pthread_mutex_lock(&mapLock);

    for(i = 1; i < 5; i++)
        UnloadTexture(gameData->mapTextures[i]);

    pthread_mutex_unlock(&mapLock);

    pthread_mutex_lock(&enemiesListLock);

    for(i = 0; i < 1; i++)
        UnloadTexture(gameData->enemiesList[i].enemyTexture);

    pthread_mutex_unlock(&enemiesListLock);
}

void LoadGameTextures(GameDataS *gameData)
{
    int ret;

    pthread_mutex_lock(&mapLock);
    ret = LoadMapTextures(&gameData->mapTextures);
    if(ret) TraceLog(LOG_INFO, "MapTextures not loaded");
    else TraceLog(LOG_DEBUG, "MapTextures loaded");
    pthread_mutex_unlock(&mapLock);

    pthread_mutex_lock(&enemiesListLock);
    ret = LoadEnemiesTextures(gameData);
    if(ret) TraceLog(LOG_INFO, "EnemiesTextures not loaded");
    else TraceLog(LOG_DEBUG, "EnemiesTextures loaded");
    pthread_mutex_unlock(&enemiesListLock);

    pthread_mutex_lock(&weaponDataLock);
    ret = LoadWeaponsTextures(gameData);
    if(ret) TraceLog(LOG_INFO, "WeaponsTextures not loaded");
    else TraceLog(LOG_DEBUG, "WeaponsTextures loaded");
    pthread_mutex_unlock(&weaponDataLock);
}

void DrawGame(GameDataS *gameData)
{
    TraceLog(LOG_DEBUG, "Saving enemies head");
    pthread_mutex_lock(&enemiesListLock);
    EnemyLL *enemiesHead = gameData->enemiesHead;
    pthread_mutex_unlock(&enemiesListLock);
    TraceLog(LOG_DEBUG, "Saving projectiles head");
    pthread_mutex_lock(&projectileListLock);
    ProjectileLL *projectileHead = gameData->projectileHead;
    pthread_mutex_unlock(&projectileListLock);
    
        ClearBackground(BLACK);
        pthread_mutex_lock(&cameraLock);
        // drawing the game
        pthread_mutex_lock(&playerLock);
        BeginMode2D(*gameData->camera);
        pthread_mutex_unlock(&cameraLock);

            // drawing map borders
            TraceLog(LOG_DEBUG, "Drawing map");
            pthread_mutex_lock(&mapLock);
            for(int i = 0; i < MAPY; i++)
            {
                for(int ii = MAPX-1; ii >= 0; ii--)
                {
                    // separating walls from floor tiles
                    if(gameData->level[i][ii] > 1)
                        DrawTexture(
                            gameData->mapTextures[gameData->level[i][ii]],
                            WALLTHICKNESS*ii,
                            WALLTHICKNESS*i,
                            WHITE
                            );
                    else
                        DrawTexture(
                            gameData->mapTextures[gameData->level[i][ii]],
                            WALLTHICKNESS*ii,
                            WALLTHICKNESS*i+12,
                            WHITE
                            );
                }
            }
            pthread_mutex_unlock(&mapLock);
                        
            // drawing player
            TraceLog(LOG_DEBUG, "Drawing player");
            DrawRectangleRec(gameData->player->player, gameData->gameSkin->primaryColor);
            pthread_mutex_unlock(&playerLock);

            // drawing projectiles
            TraceLog(LOG_DEBUG, "Drawing projectiles");
            pthread_mutex_lock(&weaponDataLock);
            pthread_mutex_lock(&projectileListLock);
            while(projectileHead->next != NULL)
            {
                projectileHead = projectileHead->next;
                DrawTexture(
                    *projectileHead->texture,
                    projectileHead->projectile.x,
                    projectileHead->projectile.y,
                    WHITE);
            }
            pthread_mutex_unlock(&weaponDataLock);
            pthread_mutex_unlock(&projectileListLock);
            
            // drawing enemies from linked list of type *EnemyLL
            TraceLog(LOG_DEBUG, "Drawing enemies");
            pthread_mutex_lock(&enemiesListLock);
            while(enemiesHead->next != NULL)
            {
                enemiesHead = enemiesHead->next;
                DrawTexture(
                    gameData->enemiesList[enemiesHead->enemyType].enemyTexture,
                    enemiesHead->enemy.x,
                    enemiesHead->enemy.y,
                    WHITE);
                // the following code draws the enemies hitbox and hp
                //DrawRectangleRec(enemiesHead->enemy, RED);
                //DrawRectangle(enemiesHead->healthBar.x,
                //              enemiesHead->healthBar.y,
                //              enemiesHead->hitPoint,
                //              enemiesHead->healthBar.height,
                //              GREEN);
            }
            pthread_mutex_unlock(&enemiesListLock);

            //DrawRectangle(gameData->mousePosition->x, gameData->mousePosition->y, 5, 5, RED);
        EndMode2D();
        // drawing the ui
        TraceLog(LOG_DEBUG, "Drawing fps");
        pthread_mutex_lock(&frameCounterLock);

        DrawFPS(5, 5);
        
        //DrawRectangle(GetMouseX(), GetMouseY(), 5, 5, YELLOW);
        pthread_mutex_unlock(&frameCounterLock);
        
        pthread_mutex_lock(&playerLock);
        
        TraceLog(LOG_DEBUG, "Drawing score and lives");
        DrawText(TextFormat("SCORE: %u", gameData->score), WALLTHICKNESS, WALLTHICKNESS, 40, WHITE);
        DrawRectangle(WALLTHICKNESS, 80, gameData->player->lives*10, 15, GREEN);

        pthread_mutex_unlock(&playerLock);

        DrawRectangle(WIDTH - 215, HEIGT - 85, 200, 50, Fade(WHITE, FADEVALUE));

        pthread_mutex_lock(&playerLock);
        pthread_mutex_lock(&weaponDataLock);

        TraceLog(LOG_DEBUG, "Drawing weapon name");
        DrawText(
            TextFormat("%d %s", gameData->player->activeWeaponId == gameData->player->weapons[0] ? 1 : 2,
            gameData->weaponsList[gameData->player->activeWeaponId].weaponName), 
            WIDTH - 200, HEIGT - 70, WALLTHICKNESS, WHITE);
            
        pthread_mutex_unlock(&playerLock);
        pthread_mutex_unlock(&weaponDataLock);
    EndDrawing();
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
        DrawFPS(5, 5);
    EndDrawing();
}