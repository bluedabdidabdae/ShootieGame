// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/cJSON.h"
#include "headers/global_types.h"

#define RAWBUFFERSIZE 2048
#define PLAYERFILE "gameData/player.json"
#define WEAPONFILE "gameData/weapons.json"
#define ENEMIESFILE "gameData/enemies.json"

int GatherPlayerData(GameDataS *gameData);
int GatherWeaponData(WeaponS **weaponsList);
int GatherEnemiesData(GameDataS *gameData);

int GatherData(GameDataS *gameData)
{
    int ret;

    TraceLog(LOG_DEBUG, "Entered GatherData func");

    ret = GatherWeaponData(&gameData->weaponsList);
    if(0 != ret) return ret;

    ret = GatherPlayerData(gameData);
    if(0 != ret) return ret;

    ret = GatherEnemiesData(gameData);
    if(0 != ret) return ret;

    return 0;
}

int GatherEnemiesData(GameDataS *gameData)
{
    FILE *rawFile;
    cJSON *enemiesData;
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
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

        // fetching weapon data
        aux2 = cJSON_GetArrayItem(aux1, i);
        if(!aux2)
        {
            strcpy(buffer, "Error loading enemies array index - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        TraceLog(LOG_DEBUG, TextFormat("Loaded enemies array index: %d", i));

        // gathering enemy size
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "size");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading enemy size - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].enemy.height = (float)aux3->valueint;
        gameData->enemiesList[i].enemy.width = (float)aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy size");
        
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
        
        // gathering enemy baseWeapon
        aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "baseWeaponId");
        if(!aux3 || !cJSON_IsNumber(aux3))
        {
            strcpy(buffer, "Error loading enemy baseWeaponId - ABORTING");
            ret = FILE_ERROR;
            goto cleanup;
        }
        gameData->enemiesList[i].baseWeaponId = aux3->valueint;
        TraceLog(LOG_DEBUG, "Loaded enemy baseWeaponId\n");
        
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