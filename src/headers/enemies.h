// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "global_types.h"

int SpawnWave(std::list<WaveEnemiesL> &enemyList, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, int **map)
int SpawnEnemies(int number, EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, LevelS &level);
int SpawnEnemiesPos(int number, EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList);
int SpawnEnemy(EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, int **map);
int SpawnEnemyPos(EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList);
void EnemiesShooting(std::list<EnemyL> &enemiesList, std::list<ProjectileL> &projectileList, EnemiesS enemiesTemplateList[], Rectangle &player);
void UpdateEnemies(std::list<EnemyL> &enemiesHead, Rectangle &player, MapS &map);

#endif