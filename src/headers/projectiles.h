// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "raylib.h"
#include "global_types.h"

void CheckProjEntityDamage(std::list<ProjectileL> &projectileList, std::list<EnemyL> &enemiesList, PlayerS &player);
void UpdateProjectiles(std::list<ProjectileL> &projectileList, LevelS &level, std::vector<CustomTexture2D> &blockList);

#endif