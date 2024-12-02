// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "raylib.h"
#include "headers/cJSON.h"
#include "headers/global_types.h"
#include "headers/gather_data.h"

#define RAWBUFFERSIZE 4096
#define MAPRAWBUFFERSIZE 16384
#define PLAYERFILE "gameData/player.json"
#define WEAPONFILE "gameData/weapons.json"
#define ENEMIESFILE "gameData/enemies.json"
#define MAPDIR "gameData/levels/level"

int GatherPlayerData(GameDataS *gameData);
int GatherWeaponData(WeaponS **weaponsList);
int GatherEnemiesData(GameDataS *gameData);

extern pthread_mutex_t enemiesListLock;
extern pthread_mutex_t projectileListLock;
extern pthread_mutex_t playerLock;
extern pthread_mutex_t gameUpdateLock;
extern pthread_mutex_t cameraLock;
extern pthread_mutex_t frameCounterLock;
extern pthread_mutex_t mapLock;
extern pthread_mutex_t weaponDataLock;

int GatherData(GameDataS *gameData)
{
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered GatherData func");
    
    pthread_mutex_lock(&weaponDataLock);
    ret = GatherWeaponData(&gameData->weaponsList);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&weaponDataLock);

    pthread_mutex_lock(&playerLock);
    ret = GatherPlayerData(gameData);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&playerLock);

    pthread_mutex_lock(&enemiesListLock);
    ret = GatherEnemiesData(gameData);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&enemiesListLock);

    return ret;
}

int LoadWeaponsTextures(GameDataS *gameData)
{
    Image tmp;
    int ret = 0;
    int i;
    char buffer[MAPRAWBUFFERSIZE];

    for(i = 0; i < 8; i++)
    {
        if(!gameData->weaponsList[i].isTexture)
        {
            tmp = gameData->weaponsList[i].projectileImage;
            ImageResize(&tmp, gameData->weaponsList[i].projectileSize, gameData->weaponsList[i].projectileSize);
            gameData->weaponsList[i].projectileTexture = LoadTextureFromImage(tmp);
            gameData->weaponsList[i].isTexture = true;
        }
    }

    cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadEnemiesTextures(GameDataS *gameData)
{
    Image tmp;
    int i = 0;
    int ret = 0;
    char buffer[MAPRAWBUFFERSIZE];

    for(i = 0; i < 2; i++)
    {
        if(!gameData->enemiesList[i].isTexture)
        {
            tmp = gameData->enemiesList[i].enemyImage;
            ImageResize(&tmp, gameData->enemiesList[i].enemy.x, gameData->enemiesList[i].enemy.y);
            gameData->enemiesList[i].enemyTexture = LoadTextureFromImage(tmp);
            UnloadImage(tmp);
            gameData->enemiesList[i].isTexture = true;
        }
        if(!gameData->enemiesList[i].weapon.isTexture)
        {
            tmp = gameData->enemiesList[i].weapon.projectileImage;
            ImageResize(&tmp, gameData->enemiesList[i].weapon.projectileSize, gameData->enemiesList[i].weapon.projectileSize);
            gameData->enemiesList[i].weapon.projectileTexture = LoadTextureFromImage(tmp);
            UnloadImage(tmp);
            gameData->enemiesList[i].weapon.isTexture = true;
        }
    }

    cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

/*
 * REALLY IMPORTANT
 * The first element of the array MUST be ignored, 
 * since it also gets ignored in all the other functions
 * wich interact with the map. In fact the 0 block is
 * the void block.
*/
int LoadMapTextures(Texture2D **mapTextures)
{
    Image tmp;
    int ret = 0;
    char buffer[MAPRAWBUFFERSIZE];

    *mapTextures = (Texture2D*)malloc(sizeof(Texture2D)*5);
    if(!*mapTextures)
    {
        strcpy(buffer, "Error allocating mapTextures memory - ABORTING");
        ret = MALLOC_ERROR;
        goto cleanup;
    }

    // Ignore this line, read the comment at the func start
    // (*mapTextures)[0]

    tmp = LoadImage("resources/floors/broken_stone_floor.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS);
    (*mapTextures)[1] = LoadTextureFromImage(tmp);
    UnloadImage(tmp);

    tmp = LoadImage("resources/walls/stone_wall.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    (*mapTextures)[2] = LoadTextureFromImage(tmp);
    UnloadImage(tmp);

    tmp = LoadImage("resources/walls/broken_stone_wall.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    (*mapTextures)[3] = LoadTextureFromImage(tmp);
    UnloadImage(tmp);

    tmp = LoadImage("resources/walls/broken_stone_wall_with_moss.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    (*mapTextures)[4] = LoadTextureFromImage(tmp);
    UnloadImage(tmp);

    
    cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadLevel(LevelS *level, int levelId)
{
    FILE *rawFile;
    cJSON *levelData;
    int ret = 0;
    char levelFile[35];
    char buffer[MAPRAWBUFFERSIZE];
    char levelid = levelId += '0';

    strcpy(levelFile, MAPDIR);
    strcat(levelFile, &levelid);
    strcat(levelFile, ".json\0");

    TraceLog(LOG_DEBUG, TextFormat("leveldir: %s", levelFile));

    // opening and parsing levels.json file
    rawFile = fopen(levelFile, "r");
    if(!rawFile)
    {
        strcpy(buffer, "Error opening levels file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Opened levels.json");
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0)
    {
        strcpy(buffer, "Error reading levels file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    fclose(rawFile);
    TraceLog(LOG_DEBUG, "Read levels.json");
    levelData = cJSON_Parse(buffer);
    if(!levelData)
    {
        strcpy(buffer, cJSON_GetErrorPtr());
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Parsed levels.json");
    
    // TODO: CHECK IF IT FAILS
    LoadMap(level->bitmap, levelData);

    // TODO: CHECK IF IT FAILS
    LoadWaves(&level->currentWave, levelData);

    cleanup:
        return ret;
}

int LoadWaves(WaveLL *currentWave, cJSON *levelData)
{
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    char buffer[RAWBUFFERSIZE];
    int i;
    int ret = 0;

    // fetching level waves
    aux1 = cJSON_GetObjectItemCaseSensitive(levelData, "waves");
    if(!aux1)
    {
        strcpy(buffer, "Error loading level waves - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded level waves");

    // load enemies
    i = 0;
    while(aux2)
    {
        aux2 = cJSON_GetArrayItem(aux1, i);
        if(!aux2)
        {
            TraceLog(LOG_DEBUG, "Loaded all waves");
            continue;
        }
        i++;
        
        // adding a wave
        currentWave->next = (WaveLL*)malloc(sizeof(WaveLL));
        currentWave = currentWave->next;
        currentWave->next = NULL;
        
        LoadWaveEnemies(&currentWave->enemies, aux1);        
    }

    cleanup:
    if(ret)
        TraceLog(LOG_ERROR, buffer);
    return ret;
}

int LoadWaveEnemies(WaveEnemiesLL *currentWaveEnemy, cJSON *currentWaveData)
{
    cJSON *aux1;
    int ret = 0;

    aux1 = cJSON_GetObjectItemCaseSensitive(currentWaveData, "minion");
    if(aux1 && cJSON_IsNumber(aux1))
    {
        currentWaveEnemy->next = (WaveEnemiesLL*)malloc(sizeof(WaveEnemiesLL));
        currentWaveEnemy = currentWaveEnemy->next;
        currentWaveEnemy->next = NULL;

        currentWaveEnemy->enemyType = MINION;
        currentWaveEnemy->nOfEnemies = aux1->valueint;
    }
    aux1 = cJSON_GetObjectItemCaseSensitive(currentWaveData, "sniper");
    if(aux1 && cJSON_IsNumber(aux1))
    {
        currentWaveEnemy->next = (WaveEnemiesLL*)malloc(sizeof(WaveEnemiesLL));
        currentWaveEnemy = currentWaveEnemy->next;
        currentWaveEnemy->next = NULL;

        currentWaveEnemy->enemyType = SNIPER;
        currentWaveEnemy->nOfEnemies = aux1->valueint;
    }

    return ret;
}

int LoadMap(int bitmap[MAPY][MAPX], cJSON *levelData)
{
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    cJSON *aux4;
    int arrSize;
    char buffer[RAWBUFFERSIZE];
    int i, ii;
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered LoadMap func");

    // fetching level matrix
    aux2 = cJSON_GetObjectItemCaseSensitive(levelData, "map");
    if(!aux2)
    {
        strcpy(buffer, "Error loading level matrix - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded level matrix");

    // fetching level y 0
    aux3 = cJSON_GetArrayItem(aux2, 0);
    if(!aux3)
    {
        strcpy(buffer, TextFormat("Error loading level y: %d - ABORTING", 0));
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, TextFormat("Loaded level y: %d", 0));

    // fetching level x 0
    aux4 = cJSON_GetArrayItem(aux3, 0);
    if(!aux3)
    {
        strcpy(buffer, TextFormat("Error loading level x: %d - ABORTING", 0));
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, TextFormat("Loaded level x: %d", 0));
    
    // loading map
    for(i = 0; i < MAPY; i++)
    {
        // fetching level x 0
        aux4 = cJSON_GetArrayItem(aux3, 0);
        if(!aux4)
        {
            strcpy(buffer, "Error loading map - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        
        // loading map
        for(ii = 0; ii < MAPX; ii++)
        {
            bitmap[i][ii] = aux4->valueint;
            aux4 = aux4->next;
        }
        aux3 = aux3->next;
    }

    cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);

        return ret;
}

int GatherEnemiesData(GameDataS *gameData)
{
    FILE *rawFile;
    cJSON *enemiesData;
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    cJSON *aux4;
    int arrSize;
    int ret = 0;
    int i;
    char buffer[RAWBUFFERSIZE];

    // opening and parsing weapons.json file
    rawFile = fopen(ENEMIESFILE, "r");
    if(!rawFile)
    {
        strcpy(buffer, "Error opening enemies file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0)
    {
        strcpy(buffer, "Error reading enemies file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    fclose(rawFile);
    enemiesData = cJSON_Parse(buffer);
    if(!enemiesData)
    {
        strcpy(buffer, cJSON_GetErrorPtr());
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Parsed enemies.json");

    // loading weapons array
    aux1 = cJSON_GetObjectItemCaseSensitive(enemiesData, "enemies");
    if(!aux1)
    {
        strcpy(buffer, "Error loading enemies array - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded enemies array");

    arrSize = cJSON_GetArraySize(aux1);

    // allocating weapons memory
    gameData->enemiesList = (EnemiesS*)malloc(sizeof(EnemiesS)*arrSize);
    if(!gameData->enemiesList)
    {
        strcpy(buffer, "Error allocating initial enemiesList memory - ABORTING");
        ret = MALLOC_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Allocated enemiesList memory");

    // initializing enemiesList
    i = 0;
    while(i < arrSize){

        // fetching enemy
        aux2 = cJSON_GetArrayItem(aux1, i);
        if(!aux2)
        {
            strcpy(buffer, "Error loading enemies array index - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        TraceLog(LOG_DEBUG, TextFormat("Loaded enemies array index: %d", i));

        // fetching enemy size
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "size");
        if(!aux3)
        {
            strcpy(buffer, "Error fetching enemy size - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        TraceLog(LOG_DEBUG, "Fetched enemy size");
        
        // gathering enemy size
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "x");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy size x - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].enemy.x = (float)aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy size x");
        
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "y");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy size y - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].enemy.y = (float)aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy size y");

        // gathering enemy baseHealth
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "baseHealth");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading enemy baseHealth - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].baseHealth = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy baseHealth");
        
        // gathering enemy image
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "enemyImage");
        if(!aux3 || !cJSON_IsString(aux3))
        {
            strcpy(buffer, "Error loading enemyImage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].enemyImage = LoadImage(aux3->valuestring);
        gameData->enemiesList[i].isTexture = false;
        TraceLog(LOG_DEBUG, "Loaded enemyImage");

        // fetching enemy weapon data
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "weapon");
        if(!aux3)
        {
            strcpy(buffer, "Error loading enemy weapon - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        TraceLog(LOG_DEBUG, "Loaded enemy weapon");

        // gathering enemy weapon damage
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "damage");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/damage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].weapon.damage = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/damage");

        // gathering enemy weapon shotsDeelay
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "shotsDeelay");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/shotsDeelay - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        gameData->enemiesList[i].weapon.shotsDeelay = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/shotsDeelay");
        
        // gathering enemy weapon projectileSize
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileSize");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileSize - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        gameData->enemiesList[i].weapon.projectileSize = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileSize");

        // gathering enemy weapon projectileSpeed
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileSpeed");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileSpeed - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        gameData->enemiesList[i].weapon.projectileSpeed = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileSpeed");

        // gathering enemy projectileImage
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileImage");
        if(!aux4 || !cJSON_IsString(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileImage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        gameData->enemiesList[i].weapon.projectileImage = LoadImage(aux4->valuestring);
        gameData->enemiesList[i].weapon.isTexture = false;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileImage\n");

        i++;
    }
    
    cleanup:

        if(ret)
            TraceLog(LOG_ERROR, buffer);

        if(enemiesData)
            cJSON_Delete(enemiesData);

        TraceLog(LOG_DEBUG, "Closed json files");

        return ret;
}

int GatherWeaponData(WeaponS **weaponsList)
{
    FILE *rawFile;
    cJSON *weaponData;
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    int i;
    int arrSize;
    int ret = 0;
    char buffer[RAWBUFFERSIZE];

    TraceLog(LOG_DEBUG, "Entered GatherWeaponData func");

    // opening and parsing weapons.json file
    rawFile = fopen(WEAPONFILE, "r");
    if(!rawFile)
    {
        strcpy(buffer, "Error opening weapons file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0)
    {
        strcpy(buffer, "Error reading weapons file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    fclose(rawFile);
    weaponData = cJSON_Parse(buffer);
    if(!weaponData)
    {
        strcpy(buffer, cJSON_GetErrorPtr());
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Parsed weapons.json");

    // loading weapons array
    aux1 = cJSON_GetObjectItemCaseSensitive(weaponData, "weapons");
    if(!aux1)
    {
        strcpy(buffer, "Error loading weapons array - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded weapons array");

    arrSize = cJSON_GetArraySize(aux1);

    // allocating weapons memory
    *weaponsList = (WeaponS*)malloc(sizeof(WeaponS)*arrSize);
    if(!*weaponsList)
    {
        strcpy(buffer, "Error allocating initial weaponsList memory - ABORTING");
        ret = MALLOC_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Allocated weaponsList memory");

    // initializing weaponsList
    i = 0;
    while(i < arrSize){

        // fetching weapon data
        aux2 = cJSON_GetArrayItem(aux1, i);
        if(!aux2)
        {
            strcpy(buffer, "Error loading weapon array index - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        TraceLog(LOG_DEBUG, TextFormat("Loaded weapon array index: %d", i));

        // gathering weapon name
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "name");
        if(!aux3 || !cJSON_IsString(aux3))
        {
            strcpy(buffer, "Error loading weapon name - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        strcpy((*weaponsList)[i].weaponName, aux3->valuestring);
        TraceLog(LOG_DEBUG, "Loaded weapon name");

        // gathering weapon damage
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "damage");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading weapon damage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*weaponsList)[i].damage = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded weapon damage");

        // gathering weapon shotsDeelay
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "shotsDeelay");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading weapon shotsDeelay - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*weaponsList)[i].shotsDeelay = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded weapon shotsDeelay");

        // gathering weapon projectileSize
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "projectileSize");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading weapon projectileSize - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*weaponsList)[i].projectileSize = (float)aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded weapon projectileSize");
        
        // gathering weapon damage
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "damage");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading weapon damage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*weaponsList)[i].damage = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded weapon damage");
        
        // gathering weapon projectileSpeed
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "projectileSpeed");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading weapon projectileSpeed - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*weaponsList)[i].projectileSpeed = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded weapon projectileSpeed\n");
        
        // gathering weapon projectileImage
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "projectileImage");
        if(!aux3 || !cJSON_IsString(aux3))
        {
            strcpy(buffer, "Error loading weapon weapon/projectileImage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        (*weaponsList)[i].projectileImage = LoadImage(aux3->valuestring);
        (*weaponsList)[i].isTexture = false;
        TraceLog(LOG_DEBUG, "Loaded weapon weapon/projectileImage\n");

        i++;
    }
    
    cleanup:

        if(ret)
            TraceLog(LOG_ERROR, buffer);

        if(weaponData)
            cJSON_Delete(weaponData);

        TraceLog(LOG_DEBUG, "Closed json files");

        return ret;
}

int GatherPlayerData(GameDataS *gameData)
{
    FILE *rawFile;
    cJSON *playerData;
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    char buffer[RAWBUFFERSIZE];
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered GatherPlayerData func");

    // opening and parsing player.json file
    rawFile = fopen(PLAYERFILE, "r");
    if(!rawFile)
    {
        strcpy(buffer, "Error opening player data file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0)
    {
        strcpy(buffer, "Error reading player data file");
        ret = FILE_ERROR;
        goto cleanup;
    }
    fclose(rawFile);

    playerData = cJSON_Parse(buffer);
    if(!playerData)
    {
        strcpy(buffer, cJSON_GetErrorPtr());
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Parsed player.json");

    // allocating player memory
    gameData->player = (PlayerS*)malloc(sizeof(PlayerS));
    if(!gameData->player)
    {
        strcpy(buffer, "Error allocating player memory - ABORTING");
        ret = MALLOC_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Allocated player memory");

    // gathering player base health
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "baseHealth");
    if(!cJSON_IsNumber(aux1) || !aux1->valueint)
    {
        strcpy(buffer, "Error loading player base health - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->lives = aux1->valueint;
    TraceLog(LOG_DEBUG, "Loaded player base health");

    // gathering player speed
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "speed");
    if(!cJSON_IsNumber(aux1) || !aux1->valueint)
    {
        strcpy(buffer, "Error loading player speed - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->speed = aux1->valueint;
    TraceLog(LOG_DEBUG, "Loaded player speed");

    // gathering player dodge data
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "dodge");
    if(!aux1)
    {
        strcpy(buffer, "Error loading player dodge data - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }

    // gathering player dodge speed
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeSpeed");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge speed - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->dodgeSpeed = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge speed");

    // gathering player dodge deelay frames
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeDeelayFrames");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge deelay frames - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->dodgeDeelayFrames = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge deelay frames");

    // gathering player dodge duration frames
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeDurationFrames");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge duration frames - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->dodgeDurationFrames = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge duration frames");

    // gathering player dodge invuln frame flag
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeInvulnFrame");
    if(!cJSON_IsBool(aux2))
    {
        strcpy(buffer, "Error loading player dodge invuln frame flag - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->flags.dodgeInvulnFrame = cJSON_IsTrue(aux2);
    TraceLog(LOG_DEBUG, "Loaded player dodge invul frame flag");

    // gathering player width and height
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "size");
    if(!aux1)
    {
        strcpy(buffer, "Error loading player size - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "x");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player width - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->player.width = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player width");

    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "y");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player height - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->player.height = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player height");
    
    // loading default player weapon
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "defaultWeaponId");
    if(!cJSON_IsNumber(aux1) || 0 > aux1->valueint)
    {
        strcpy(buffer, "Error loading player base weapon - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    gameData->player->weapons[0] = aux1->valueint;
    gameData->player->weapons[1] = -1;
    gameData->player->activeWeaponId = gameData->player->weapons[0];
    TraceLog(LOG_DEBUG, "Loaded player base weapon");

    cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);

        if(playerData)
            cJSON_Delete(playerData);

        TraceLog(LOG_DEBUG, "Closed json files");

        return ret;
}