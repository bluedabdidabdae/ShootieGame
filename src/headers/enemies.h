// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "global_types.h"

int SpawnWave(std::list<WaveEnemiesL> &enemyList, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, MapS &map, std::vector<CustomTexture2D> &blockList);
int SpawnEnemies(int number, EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, MapS &map, std::vector<CustomTexture2D> &blockList);
int SpawnEnemiesPos(int number, EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList);
int SpawnEnemy(EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, MapS &map, std::vector<CustomTexture2D> &blockList);
int SpawnEnemyPos(EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList);
void EnemiesShooting(std::list<EnemyL> &enemiesList, std::list<ProjectileL> &projectileList, EnemiesS enemiesTemplateList[], Rectangle &player);
void UpdateEnemies(std::list<EnemyL> &enemiesList, Rectangle &player, MapS &map, std::vector<CustomTexture2D> &blockList);

#endif