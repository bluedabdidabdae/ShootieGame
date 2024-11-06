// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdio.h>
#include <stdlib.h>

#include "headers/cJSON.h"
#include "headers/global_types.h"

#define RAWBUFFERSIZE 2048
#define PLAYERFILE "gameData/player.json"
#define WEAPONFILE "gameData/weapons.json"

int GatherPlayerData(GameDataS *gameData);

int GatherData(GameDataS *gameData)
{
    int err;

    TraceLog(LOG_DEBUG, "Entered GatherData func");

    err = GatherPlayerData(gameData);
    if(0 != err) return err;

    return 0;
}

int GatherPlayerData(GameDataS *gameData)
{
    FILE *rawFile;
    cJSON *playerData;
    cJSON *weaponData;
    cJSON *aux1;
    cJSON *aux2;
    cJSON *aux3;
    char buffer[RAWBUFFERSIZE];

    TraceLog(LOG_DEBUG, "Entered GatherPlayerData func");

    // opening and parsing player.json file
    rawFile = fopen(PLAYERFILE, "r");
    if(!rawFile) return FILE_ERROR;
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0) return FILE_ERROR;
    fclose(rawFile);

    playerData = cJSON_Parse(buffer);
    if(!playerData)
    {
        const char *errorPtr = cJSON_GetErrorPtr();
        TraceLog(LOG_ERROR, "Json error:");
        if(!errorPtr)
            TraceLog(LOG_ERROR, errorPtr);
        cJSON_Delete(playerData);
        return FILE_ERROR;
    }
    TraceLog(LOG_DEBUG, "Parsed player.json");

    // opening and parsing weapons.json file
    rawFile = fopen(WEAPONFILE, "r");
    if(!rawFile) return FILE_ERROR;
    if(fread(buffer, 1, sizeof(buffer), rawFile) < 0) return FILE_ERROR;
    fclose(rawFile);

    weaponData = cJSON_Parse(buffer);
    if(!weaponData)
    {
        const char *errorPtr = cJSON_GetErrorPtr();
        TraceLog(LOG_ERROR, "Json error:");
        if(!errorPtr)
            TraceLog(LOG_ERROR, errorPtr);
        cJSON_Delete(weaponData);
        return FILE_ERROR;
    }
    TraceLog(LOG_DEBUG, "Parsed weapons.json");

    // allocating player memory
    gameData->player = (PlayerS*)malloc(sizeof(PlayerS));
    if(!gameData->player)
    {
        TraceLog(LOG_ERROR, "Error allocating player memory - ABORTING");
        return MALLOC_ERROR;
    }
    TraceLog(LOG_DEBUG, "Allocated player memory");

    // gathering player base health
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "baseHealth");
    if(!cJSON_IsNumber(aux1) || !aux1->valueint)
    {
        TraceLog(LOG_ERROR, "Error loading player base health - ABORTING");
        return FILE_ERROR;
    }
    gameData->player->lives = aux1->valueint;
    TraceLog(LOG_DEBUG, "Loaded player base health");


    // gathering player width and height
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "size");
    if(!aux1)
    {
        TraceLog(LOG_ERROR, "Error loading player size - ABORTING");
        return FILE_ERROR;
    }
    
    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "x");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        TraceLog(LOG_ERROR, "Error loading player width - ABORTING");
        return FILE_ERROR;
    }
    gameData->player->player.width = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player width");

    aux2 = cJSON_GetObjectItemCaseSensitive(aux1, "y");
    if(!cJSON_IsNumber(aux2) || !aux2->valueint)
    {
        TraceLog(LOG_ERROR, "Error loading player height - ABORTING");
        return FILE_ERROR;
    }
    gameData->player->player.height = aux2->valueint;
    TraceLog(LOG_DEBUG, "Loaded player height");
    
    // loading default player weapon
    aux1 = cJSON_GetObjectItemCaseSensitive(playerData, "defaultWeapon");
    if(!cJSON_IsString(aux1) || !aux1->valuestring)
    {
        TraceLog(LOG_ERROR, "Error loading player base weapon - ABORTING");
        return FILE_ERROR;
    }
    aux2 = cJSON_GetObjectItemCaseSensitive(weaponData, aux1->valuestring);
    if(!aux2)
    {
        TraceLog(LOG_ERROR, "Error fetching player base weapon - ABORTING");
        return FILE_ERROR;
    }
    // if program gets here specified weapon exists
    aux3 = cJSON_GetObjectItemCaseSensitive(aux2, "weaponId");
    if(!cJSON_IsNumber(aux3) || !aux3->valueint)
    {
        TraceLog(LOG_ERROR, "Error loading player base weapon id - ABORTING");
        return FILE_ERROR;
    }
    gameData->player->weapons[0] = aux3->valueint;
    gameData->player->weapons[1] = 0;
    gameData->player->activeWeapon = 0;
    TraceLog(LOG_DEBUG, "Loaded player base weapon");

    cJSON_Delete(playerData);
    cJSON_Delete(weaponData);
    TraceLog(LOG_DEBUG, "Closed json files");

    return 0;
}