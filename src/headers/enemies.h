// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "global_types.h"

int SpawnEnemies(GameDataS *gameData, int number, EnemyType enemyType);
int SpawnEnemy(GameDataS &gameData, float x, float y, EnemyType enemyType);
int SpawnEnemy(GameDataS &gameData, EnemyType enemyType);
void EnemiesShooting(std::list<EnemyL> &enemiesList, std::list<ProjectileL> &projectileList, EnemiesS enemiesTemplateList[], Rectangle *player);
void UpdateEnemies(std::list<EnemyL> &enemiesHead, Rectangle *player, int level[MAPY][MAPX]);

#endif