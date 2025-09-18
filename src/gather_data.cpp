// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iterator>
#include <list>
#include <vector>

#include "raylib.h"
#include "headers/cJSON.h"
#include "headers/global_types.h"
#include "headers/gather_data.h"

#define RAWBUFFERSIZE 3000
#define MAPRAWBUFFERSIZE 4900
#define PLAYERFILE "gameData/player.json"
#define WEAPONFILE "gameData/weapons.json"
#define ENEMIESFILE "gameData/enemies.json"
#define MAPDIR "gameData/levels/level"

int GatherWeaponData(WeaponS **weaponsList);
int GatherPlayerData(PlayerS &player);
int GatherEnemiesData(EnemiesS **enemiesTemplateList);
int LoadMap(MapS &map, cJSON *levelData);
int LoadWaves(std::list<WaveL> &waveList, cJSON *levelData);
int LoadWaveEnemies(std::list<WaveEnemiesL> &waveNemeyList, cJSON *currentWaveData);

int GatherData(GameDataS &gameData)
{
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered GatherData func");
    
    pthread_mutex_lock(&weaponDataLock);
    ret = GatherWeaponData(&gameData.weaponsList);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&weaponDataLock);

    pthread_mutex_lock(&playerLock);
    ret = GatherPlayerData(gameData.player);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&playerLock);

    pthread_mutex_lock(&enemiesListLock);
    ret = GatherEnemiesData(&gameData.enemiesTemplateList);
    if(0 != ret) return ret;
    pthread_mutex_unlock(&enemiesListLock);

    return ret;
}

int LoadPlayerTexture(PlayerS *player)
{
    Image tmp;
    int ret = 0;
    char buffer[MAPRAWBUFFERSIZE] = { 0 };

    tmp = player->playerImage;
    ImageResize(&tmp, player->player.width, player->player.height);
    player->playerTexture = LoadTextureFromImage(tmp);
    player->isTexture = true;
    
    unused cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadWeaponsTextures(WeaponS *weaponsList)
{
    Image tmp;
    int ret = 0;
    int i;
    char buffer[MAPRAWBUFFERSIZE] = { 0 };

    for(i = 0; i < 8; i++)
    {
        if(!weaponsList[i].isTexture)
        {
            tmp = weaponsList[i].projectileImage;
            ImageResize(&tmp, weaponsList[i].projectileSize, weaponsList[i].projectileSize);
            weaponsList[i].projectileTexture = LoadTextureFromImage(tmp);
            weaponsList[i].isTexture = true;
        }
    }
    
    unused cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadEnemiesTextures(EnemiesS *enemiesTemplateList)
{
    Image tmp;
    int ret = 0;

    if(!enemiesTemplateList)
    {
        TraceLog(LOG_FATAL, "Enemies template list is NULL - FATAL");
        return MALLOC_ERROR;
    }

    for(int i = 0; i < 2; i++)
    {
        if(!enemiesTemplateList[i].isTexture)
        {
            tmp = enemiesTemplateList[i].enemyImage;
            ImageResize(&tmp, enemiesTemplateList[i].enemy.x, enemiesTemplateList[i].enemy.y);
            enemiesTemplateList[i].enemyTexture = LoadTextureFromImage(tmp);
            UnloadImage(tmp);
            enemiesTemplateList[i].isTexture = true;
        }
        if(!enemiesTemplateList[i].weapon.isTexture)
        {
            tmp = enemiesTemplateList[i].weapon.projectileImage;
            ImageResize(&tmp, enemiesTemplateList[i].weapon.projectileSize, enemiesTemplateList[i].weapon.projectileSize);
            enemiesTemplateList[i].weapon.projectileTexture = LoadTextureFromImage(tmp);
            UnloadImage(tmp);
            enemiesTemplateList[i].weapon.isTexture = true;
        }
        TraceLog(LOG_DEBUG, TextFormat("Loaded enemy template texture n.%d", i));
    }

    unused cleanup:
        return ret;
}

int LoadMapTextures(std::vector<CustomTexture2D> &mapTextures)
{
    Image tmp;
    int ret = 0;
    char buffer[MAPRAWBUFFERSIZE] = { 0 };
    CustomTexture2D tmpTexture2D;

    TraceLog(LOG_DEBUG, "Entered LoadMapTextures func");

    tmp = LoadImage("resources/not_found.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    // this block's hitbox is actually hardcoded to true in CheckHitboxMap(...) at utility.cpp
    tmpTexture2D.blockType = SPECIAL_HITB;
    mapTextures.push_back(tmpTexture2D);

    tmp = LoadImage("resources/void_block.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    tmpTexture2D.blockType = SPECIAL_HITB;
    mapTextures.push_back(tmpTexture2D);

    tmp = LoadImage("resources/floors/broken_stone_floor.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    tmpTexture2D.blockType = FLOOR_NO_HITB;
    mapTextures.push_back(tmpTexture2D);

    tmp = LoadImage("resources/walls/stone_wall.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    tmpTexture2D.blockType = WALL_HITB;
    mapTextures.push_back(tmpTexture2D);

    tmp = LoadImage("resources/walls/broken_stone_wall.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    tmpTexture2D.blockType = WALL_HITB;
    mapTextures.push_back(tmpTexture2D);

    tmp = LoadImage("resources/walls/broken_stone_wall_with_moss.png");
    ImageResize(&tmp, WALLTHICKNESS, WALLTHICKNESS+12);
    tmpTexture2D.texture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    tmpTexture2D.blockType = WALL_HITB;
    mapTextures.push_back(tmpTexture2D);

    unused cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadLevel(LevelS &level, int levelId)
{
    FILE *rawFile;
    cJSON *levelData;
    int ret = 0;
    char levelFile[35];
    char buffer[MAPRAWBUFFERSIZE] = { 0 };
    char levelIdCh = levelId + '0';

    TraceLog(LOG_DEBUG, "Entered LoadLevel func");

    level.levelId = levelId;

    strcpy(levelFile, MAPDIR);
    strcat(levelFile, &levelIdCh);
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
    TraceLog(LOG_DEBUG, "Opened levelx.json");
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0)
    {
        strcpy(buffer, "Error reading levelx.json");
        ret = FILE_ERROR;
        goto cleanup;
    }
    fclose(rawFile);
    TraceLog(LOG_DEBUG, "Read levelx.json");
    levelData = cJSON_Parse(buffer);
    if(!levelData)
    {
        strcpy(buffer, cJSON_GetErrorPtr());
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Parsed levelx.json");
    
    ret = LoadMap(level.map, levelData);
    if(ret)
    {
        strcpy(buffer, "Error loading map");
        ret = FILE_ERROR;
        goto cleanup;
    }
    
    // TODO: CHECK IF IT FAILS
    LoadWaves(level.waveList, levelData);

    unused cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);
        return ret;
}

int LoadWaves(std::list<WaveL> &waveList, cJSON *levelData)
{
    cJSON *aux1;
    cJSON *aux2;
    char buffer[RAWBUFFERSIZE] = { 0 };
    int i;
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered LoadWaves func");

    // fetching level waves
    aux1 = cJSON_GetObjectItemCaseSensitive(levelData, "waves");
    if(!aux1)
    {
        strcpy(buffer, "Error fetching level waves - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Fetched level waves");

    // load enemies
    i = 0;
    do{
        aux2 = cJSON_GetArrayItem(aux1, i);
        if(!aux2)
        {
            TraceLog(LOG_DEBUG, "Loaded all waves");
            continue;
        }
        i++;
        
        WaveL tmpWave;
        LoadWaveEnemies(tmpWave.enemyList, aux2);        
        waveList.push_back(tmpWave);
        
        TraceLog(LOG_DEBUG, TextFormat("Loaded wave n.%d", i));
    }while(aux2);
    
    unused cleanup:
    if(ret)
        TraceLog(LOG_ERROR, buffer);
    return ret;
}

int LoadWaveEnemies(std::list<WaveEnemiesL> &waveEnemyList, cJSON *currentWaveData)
{
    cJSON *aux1;
    int ret = 0;
    WaveEnemiesL tmpWaveEnemy;

    aux1 = cJSON_GetObjectItemCaseSensitive(currentWaveData, "minion");
    if(aux1 && cJSON_IsNumber(aux1))
    {
        tmpWaveEnemy.enemyType = MINION;
        tmpWaveEnemy.nOfEnemies = aux1->valueint;
        waveEnemyList.push_back(tmpWaveEnemy);
    }
    aux1 = cJSON_GetObjectItemCaseSensitive(currentWaveData, "sniper");
    if(aux1 && cJSON_IsNumber(aux1))
    {
        tmpWaveEnemy.enemyType = SNIPER;
        tmpWaveEnemy.nOfEnemies = aux1->valueint;
        waveEnemyList.push_back(tmpWaveEnemy);
    }

    return ret;
}

int LoadMap(MapS &map, cJSON *levelData)
{
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    char buffer[RAWBUFFERSIZE] = { 0 };
    int ret = 0;

    TraceLog(LOG_DEBUG, "Entered LoadMap func");

    // fetching level matrix
    aux1 = cJSON_GetObjectItemCaseSensitive(levelData, "map");
    if(!aux1)
    {
        strcpy(buffer, "Error loading map matrix - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded map matrix");

    // fetching level y 0
    aux2 = cJSON_GetArrayItem(aux1, 0);
    if(!aux2)
    {
        strcpy(buffer, TextFormat("Error loading map y: %d - ABORTING", 0));
        ret = FILE_ERROR;
        goto cleanup;
    }
    map.sizeY = cJSON_GetArraySize(aux1);
    TraceLog(LOG_DEBUG, "Loaded map y: 0");

    // fetching level x 0
    aux3 = cJSON_GetArrayItem(aux2, 0);
    if(!aux2)
    {
        strcpy(buffer, TextFormat("Error loading map x: %d - ABORTING", 0));
        ret = FILE_ERROR;
        goto cleanup;
    }
    map.sizeX = cJSON_GetArraySize(aux2);
    TraceLog(LOG_DEBUG, "Loaded map x: 0");

    map.bitmap = new uint*[map.sizeY];
    for(uint i = 0; i < map.sizeY; i++)
        map.bitmap[i] = new uint[map.sizeX];

    TraceLog(LOG_DEBUG, "Created map matrix");

    for(int i = 0; aux2; i++)
    {
        aux3 = cJSON_GetArrayItem(aux2, 0);
        if(!aux3)
        {
            strcpy(buffer, cJSON_GetErrorPtr());
            ret = FILE_ERROR;
            goto cleanup;
        }
        
        for(int ii = 0; aux3; ii++)
        {
            map.bitmap[i][ii] = aux3->valueint;
            aux3 = aux3->next;
        }
        aux2 = aux2->next;
    }

    TraceLog(LOG_DEBUG, TextFormat("Mapsize Y: %d", map.sizeY));
    TraceLog(LOG_DEBUG, TextFormat("Mapsize X: %d", map.sizeX));

    unused cleanup:
        if(ret)
        {
            TraceLog(LOG_ERROR, buffer);
            if(map.bitmap)
                free(map.bitmap);
        }
        
        return ret;
}

int GatherEnemiesData(EnemiesS **enemiesTemplateList)
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
    char buffer[RAWBUFFERSIZE] = { 0 };

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

    // allocating enemies memory
    *enemiesTemplateList = new EnemiesS[arrSize];
    if(!enemiesTemplateList)
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
        (*enemiesTemplateList)[i].enemy.x = (float)aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy size x");
        
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "y");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy size y - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*enemiesTemplateList)[i].enemy.y = (float)aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy size y");

        // gathering enemy baseHealth
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "baseHealth");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading enemy baseHealth - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*enemiesTemplateList)[i].baseHealth = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy baseHealth");
        
        // gathering enemy image
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "enemyImage");
        if(!aux3 || !cJSON_IsString(aux3))
        {
            strcpy(buffer, "Error loading enemyImage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        (*enemiesTemplateList)[i].enemyImage = LoadImage(aux3->valuestring);
        (*enemiesTemplateList)[i].isTexture = false;
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
        (*enemiesTemplateList)[i].weapon.damage = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/damage");

        // gathering enemy weapon shotsDeelay
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "shotsDeelay");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/shotsDeelay - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        (*enemiesTemplateList)[i].weapon.shotsDeelay = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/shotsDeelay");
        
        // gathering enemy weapon projectileSize
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileSize");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileSize - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        (*enemiesTemplateList)[i].weapon.projectileSize = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileSize");

        // gathering enemy weapon projectileSpeed
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileSpeed");
        if(!aux4 || !cJSON_IsNumber(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileSpeed - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        (*enemiesTemplateList)[i].weapon.projectileSpeed = aux4->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileSpeed");

        // gathering enemy projectileImage
        aux4 = cJSON_GetObjectItemCaseSensitive(aux3, "projectileImage");
        if(!aux4 || !cJSON_IsString(aux4))
        {
            strcpy(buffer, "Error loading enemy weapon/projectileImage - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }

        (*enemiesTemplateList)[i].weapon.projectileImage = LoadImage(aux4->valuestring);
        (*enemiesTemplateList)[i].weapon.isTexture = false;
        TraceLog(LOG_DEBUG, "Loaded enemy weapon/projectileImage\n");

        i++;
    }
    
    unused cleanup:

        if(ret)
        {
            TraceLog(LOG_ERROR, buffer);
            if(*enemiesTemplateList)
                free(*enemiesTemplateList);
        }

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
    char buffer[RAWBUFFERSIZE] = { 0 };

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
    *weaponsList = new WeaponS[arrSize];
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
    
    unused cleanup:

        if(ret)
            TraceLog(LOG_ERROR, buffer);

        if(weaponData)
            cJSON_Delete(weaponData);

        TraceLog(LOG_DEBUG, "Closed json files");

        return ret;
}

int GatherPlayerData(PlayerS &player)
{
    FILE *rawFile;
    cJSON *playerData;
    cJSON *aux1;
    cJSON *aux2;
    char buffer[RAWBUFFERSIZE] = { 0 };
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

    // gathering player base health
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "baseHealth");
    if(!cJSON_IsNumber(aux1) || !aux1->valueint)
    {
        strcpy(buffer, "Error loading player base health - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.lives = aux1->valueint;
    TraceLog(LOG_DEBUG, "Loaded player base health");

    // gathering player speed
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "speed");
    if(!cJSON_IsNumber(aux1) || !aux1->valueint)
    {
        strcpy(buffer, "Error loading player speed - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.speed = aux1->valueint;
    TraceLog(LOG_DEBUG, "Loaded player speed");

    // gathering player dodge data
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "dodge");
    if(!aux1)
    {
        strcpy(buffer, "Error loading player dodge data - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    TraceLog(LOG_DEBUG, "Loaded player dodge data");

    // gathering player dodge speed
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeSpeed");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge speed - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.dodgeSpeed = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge speed");

    // gathering player dodge deelay frames
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeDeelayFrames");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge deelay frames - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.dodgeDeelayFrames = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge deelay frames");

    // gathering player dodge duration frames
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeDurationFrames");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player dodge duration frames - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.dodgeDurationFrames = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player dodge duration frames");

    // gathering player dodge invuln frame flag
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "dodgeInvulnFrame");
    if(!cJSON_IsBool(aux2))
    {
        strcpy(buffer, "Error loading player dodge invuln frame flag - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.flags.dodgeInvulnFrame = cJSON_IsTrue(aux2);
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
    player.player.width = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player width");

    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "y");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        strcpy(buffer, "Error loading player height - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.player.height = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player height");
    
    // loading default player weapon
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "defaultWeaponId");
    if(!cJSON_IsNumber(aux1) || 0 > aux1->valueint)
    {
        strcpy(buffer, "Error loading player base weapon - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.weapons[0] = aux1->valueint;
    player.weapons[1] = -1;
    player.activeWeaponId = player.weapons[0];
    TraceLog(LOG_DEBUG, "Loaded player base weapon");

    // loading player image
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "playerImage");
    if(!cJSON_IsString(aux1))
    {
        strcpy(buffer, "Error loading player image - ABORTING");
        ret = FILE_ERROR;
        goto cleanup;
    }
    player.playerImage = LoadImage(aux1->valuestring);
    player.isTexture = false;
    TraceLog(LOG_DEBUG, "Loaded player image");

    unused cleanup:
        if(ret)
            TraceLog(LOG_ERROR, buffer);

        if(playerData)
            cJSON_Delete(playerData);

        TraceLog(LOG_DEBUG, "Closed json files");

        return ret;
}