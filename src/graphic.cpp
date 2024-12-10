// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <pthread.h>
#include <iterator>

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
void LoadGameTextures(GameDataS &gameData);
void UnloadGameTextures(GameDataS &gameData);
void DrawMenuLoop(ToDraw &toDraw);
void DrawMenu();
void DrawSettingsLoop(ToDraw &toDraw, SettingsFlags &settingsFlags);
void DrawSettings();
void DrawGameLoop(AppDataS &appData);
void DrawGame(GameDataS &gameData);

void *HandleGraphics(void *data)
{
    int ret = 0;
    AppDataS &appData = *(AppDataS*)data;

    InitWindow(WIDTH, HEIGT, WINDOWNAME);
    SetTargetFPS(TARGETFPS);

    // sending clear to update to main
    pthread_mutex_unlock(&gameUpdateLock);

    while(appData.toDraw != DRAWCLOSEGAME)
    {
        switch(appData.toDraw)
        {
            case DRAW_WAIT: break;
            case DRAWMAINMENU:
                DrawMenuLoop(appData.toDraw);
            break;
            case DRAWSETTINGS:
                DrawSettingsLoop(appData.toDraw, appData.settingsFlags);
            break;
            case DRAW_LOAD_TEXTURES:
                LoadGameTextures(*appData.gameData);
                appData.toDraw = DRAW_WAIT;
            break;
            case DRAWGAME:
                DrawGameLoop(appData);
            break;
            case DRAW_UNLOAD_TEXTURES:
                UnloadGameTextures(*appData.gameData);
                appData.toDraw = DRAW_WAIT;
            break;
            case DRAWABORT:
                TraceLog(LOG_DEBUG, "<< Aborting on drawing thread >>");
                abort();
            break;
        }
    }

    CloseWindow();
    pthread_exit(NULL);
    return NULL;
}

void DrawMenuLoop(ToDraw &toDraw)
{
    do{
        pthread_mutex_unlock(&gameUpdateLock);
        DrawMenu();
    }while(DRAWMAINMENU == toDraw);
}

void DrawSettingsLoop(ToDraw &toDraw, SettingsFlags &settingsFlags)
{
    do{
        pthread_mutex_unlock(&gameUpdateLock);
        DrawSettings();

        if(settingsFlags.toggleFullscreen)
        {
            ToggleFullscreen();
            settingsFlags.toggleFullscreen = false;
        }
    }while(DRAWSETTINGS == toDraw);
}

void DrawGameLoop(AppDataS &appData)
{
    while(DRAWGAME == appData.toDraw)
    {
        //sending clear to update to game engine
        pthread_mutex_unlock(&gameUpdateLock);
        DrawGame(*appData.gameData);

        pthread_mutex_lock(&frameCounterLock);
        TraceLog(LOG_DEBUG, "Updating frame counter");
        appData.gameData->frameCounter += 1;
        pthread_mutex_unlock(&frameCounterLock);
    }
}

void UnloadGameTextures(GameDataS &gameData)
{
    int i;

    TraceLog(LOG_DEBUG, "Unloading projectile textures from weapons list");
    pthread_mutex_lock(&weaponDataLock);
    for(i = 0; i < 7; i++)
        UnloadTexture(gameData.weaponsList[i].projectileTexture);
    pthread_mutex_unlock(&weaponDataLock);

    TraceLog(LOG_DEBUG, "Unloading textures from enemies list");
    pthread_mutex_lock(&enemiesListLock);
    for(i = 0; i < 2; i++)
    {
        UnloadTexture(gameData.enemiesTemplateList[i].weapon.projectileTexture);
        UnloadTexture(gameData.enemiesTemplateList[i].enemyTexture);
    }
    pthread_mutex_unlock(&enemiesListLock);

    TraceLog(LOG_DEBUG, "Unloading tiles textures from map list");
    pthread_mutex_lock(&mapLock);
    for(i = 1; i < 5; i++)
        UnloadTexture(gameData.mapTextures[i]);
    pthread_mutex_unlock(&mapLock);

    pthread_mutex_unlock(&gameUpdateLock);
}

void LoadGameTextures(GameDataS &gameData)
{
    int ret;

    pthread_mutex_lock(&mapLock);
    ret = LoadMapTextures(&gameData.mapTextures);
    if(ret) TraceLog(LOG_INFO, "MapTextures not loaded");
    else TraceLog(LOG_DEBUG, "MapTextures loaded");
    pthread_mutex_unlock(&mapLock);

    pthread_mutex_lock(&enemiesListLock);
    ret = LoadEnemiesTextures(gameData.enemiesTemplateList);
    if(ret) TraceLog(LOG_INFO, "EnemiesTextures not loaded");
    else TraceLog(LOG_DEBUG, "EnemiesTextures loaded");
    pthread_mutex_unlock(&enemiesListLock);

    pthread_mutex_lock(&weaponDataLock);
    ret = LoadWeaponsTextures(gameData.weaponsList);
    if(ret) TraceLog(LOG_INFO, "WeaponsTextures not loaded");
    else TraceLog(LOG_DEBUG, "WeaponsTextures loaded");
    pthread_mutex_unlock(&weaponDataLock);

    pthread_mutex_unlock(&gameUpdateLock);
}

void DrawGame(GameDataS &gameData)
{
        ClearBackground(BLACK);
        pthread_mutex_lock(&cameraLock);
        // drawing the game
        pthread_mutex_lock(&playerLock);
        BeginMode2D(gameData.camera);
        pthread_mutex_unlock(&cameraLock);

            // drawing map borders
            TraceLog(LOG_DEBUG, "Drawing map");
            pthread_mutex_lock(&mapLock);
            for(int i = 0; i < gameData.level.map.sizeY; i++)
            {
                for(int ii = gameData.level.map.sizeX-1; ii >= 0; ii--)
                {
                    // separating walls from floor tiles
                    if(gameData.level.map.bitmap[i][ii] > 1)
                        DrawTexture(
                            gameData.mapTextures[gameData.level.map.bitmap[i][ii]],
                            WALLTHICKNESS*ii,
                            WALLTHICKNESS*i,
                            WHITE
                            );
                    else
                        DrawTexture(
                            gameData.mapTextures[gameData.level.map.bitmap[i][ii]],
                            WALLTHICKNESS*ii,
                            WALLTHICKNESS*i+12,
                            WHITE
                            );
                }
            }
            pthread_mutex_unlock(&mapLock);
                        
            // drawing player
            TraceLog(LOG_DEBUG, "Drawing player");
            if(!gameData.player.flags.isDodging)
                DrawRectangleRec(gameData.player.player, BLUE);
            else
                DrawRectangleRec(gameData.player.player, RED);
            pthread_mutex_unlock(&playerLock);

            // drawing projectiles
            TraceLog(LOG_DEBUG, "Drawing projectiles");
            pthread_mutex_lock(&weaponDataLock);
            pthread_mutex_lock(&projectileListLock);
            for(auto const& currentProjectile : gameData.projectileList)
            {
                DrawTexture(
                    *currentProjectile.texture,
                    currentProjectile.projectile.x,
                    currentProjectile.projectile.y,
                    WHITE);
            }
            pthread_mutex_unlock(&weaponDataLock);
            pthread_mutex_unlock(&projectileListLock);
            
            // drawing enemies
            TraceLog(LOG_DEBUG, "Drawing enemies");
            pthread_mutex_lock(&enemiesListLock);
            for(auto const& currentEnemy : gameData.enemiesList)
            {
                DrawTexture(
                    gameData.enemiesTemplateList[currentEnemy.enemyType].enemyTexture,
                    currentEnemy.enemy.x,
                    currentEnemy.enemy.y,
                    WHITE);
                // the following code draws the enemies hitbox and hp
                //DrawRectangleRec(currentEnemy.enemy, RED);
                //DrawRectangle(currentEnemy.healthBar.x,
                //              currentEnemy.healthBar.y,
                //              currentEnemy.hitPoint,
                //              currentEnemy.healthBar.height,
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
        DrawText(TextFormat("SCORE: %u", gameData.score), 30, 30, 40, WHITE);
        DrawRectangle(30, 80, gameData.player.lives*10, 15, GREEN);

        pthread_mutex_unlock(&playerLock);

        DrawRectangle(1130, 590, 200, 50, Fade(WHITE, FADEVALUE));

        pthread_mutex_lock(&playerLock);
        pthread_mutex_lock(&weaponDataLock);

        TraceLog(LOG_DEBUG, "Drawing weapon name");
        DrawText(
            TextFormat("%d %s", gameData.player.activeWeaponId == gameData.player.weapons[0] ? 1 : 2,
            gameData.weaponsList[gameData.player.activeWeaponId].weaponName), 
            1170, 600, 30, WHITE);
            
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
        DrawText(TextFormat("Settings"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+105, 40, MAINMENUTEXTCOLOR);

        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY+200, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Exit"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+205, 40, MAINMENUTEXTCOLOR);
        DrawFPS(5, 5);
    EndDrawing();
}

void DrawSettings()
{
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("Settings"), MAINMENUBUTTONX, 100, 70, MAINMENUTEXTCOLOR);
    
        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Toggle fullscreen"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+5, 40, MAINMENUTEXTCOLOR);

        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY+100, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Back"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+105, 40, MAINMENUTEXTCOLOR);

        DrawRectangle(MAINMENUBUTTONX, MAINMENUBUTTONY+200, MAINMENUBUTTONWIDTH, MAINMENUBUTTONHEIGT, Fade(MAINMENUTEXTCOLOR, FADEVALUE));
        DrawText(TextFormat("Quit game"), MAINMENUBUTTONX+330, MAINMENUBUTTONY+205, 40, MAINMENUTEXTCOLOR);
        DrawFPS(5, 5);
    EndDrawing();
}