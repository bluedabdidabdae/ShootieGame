// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PLAYER_H
#define PLAYER_H

#define PLAYERSPEED 3.5f

#include "raylib.h"
#include "global_types.h"

void PlayerShooting(PlayerS *player, WeaponS weaponsList[], ProjectileLL *projectileHead, Vector2 *mousePosition);
void UpdatePlayer(PlayerS *player, int level[MAPY][MAPX]);

#endif