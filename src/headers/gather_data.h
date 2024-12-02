// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GATHER_DATA_H
#define GATHER_DATA_H

#include "cJSON.h"

int GatherData(GameDataS *gameData);
int LoadLevel(LevelS *level, int levelId);
int LoadMap(int bitmap[MAPY][MAPX], cJSON *levelData);
int LoadWaves(WaveLL *currentWave, cJSON *levelData);
int LoadWaveEnemies(WaveEnemiesLL *currentWaveEnemy, cJSON *currentWaveData);
int LoadMapTextures(Texture2D **mapTextures);
int LoadEnemiesTextures(GameDataS *gameData);
int LoadWeaponsTextures(GameDataS *gameData);

#endif