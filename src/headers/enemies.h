// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H

#define ENEMYSPEED 3.0f
#define ENEMYMAXPDISTANCE 500
#define ENEMYMINPDISTANCE 400

#include "graphic.h"

void EnemyPop(EnemyLL *prePop, EnemyLL **toPop);
int SpawnEnemy(EnemyLL *destination, float x, float y);
void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[]);
void EnemiesShooting(EnemyLL *currentEnemy, ProjectileLL *projectileHead, Rectangle *player);
void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player);
void CompletelyDeleteAllEnemies(EnemyLL *head);

#endif