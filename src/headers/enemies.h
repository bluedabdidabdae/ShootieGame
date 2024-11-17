// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "global_types.h"


int SpawnEnemies(GameDataS *gameData, int number, EnemyType enemyType);
int SpawnEnemy(GameDataS *gameData, float x, float y, EnemyType enemyType);
int SpawnEnemy(GameDataS *gameData, EnemyType enemyType);
void SnapEnemies(GameDataS *gameData);
void EnemiesShooting(GameDataS *gameData);
void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player, int level[MAPY][MAPX]);
void CompletelyDeleteAllEnemies(EnemyLL *head);

#endif