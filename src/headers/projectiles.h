// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "raylib.h"
#include "global_types.h"

void CheckProjEntityDamage(GameDataS *gameData);
void UpdateProjectiles(ProjectileLL *projectileHead);
void CheckProjectilesBorders(ProjectileLL *projectileHead, Rectangle mapBorder[]);
void CompletelyDeleteAllProjectiles(ProjectileLL *head);

#endif