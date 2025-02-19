// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GATHER_DATA_H
#define GATHER_DATA_H

#include "cJSON.h"

int GatherData(GameDataS &gameData);
int LoadLevel(LevelS &level, int levelId);
int LoadMapTextures(std::vector<CustomTexture2D> &mapTextures);
int LoadPlayerTexture(PlayerS *player);
int LoadEnemiesTextures(EnemiesS *enemiesTemplateList);
int LoadWeaponsTextures(WeaponS *weaponsList);

#endif