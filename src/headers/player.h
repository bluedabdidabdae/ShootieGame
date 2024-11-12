// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PLAYER_H
#define PLAYER_H

#define PLAYERSPEED 6.0f

#include "raylib.h"
#include "global_types.h"

void PlayerShooting(GameDataS *gameData);
void UpdatePlayer(PlayerS *player, int level[MAPY][MAPX]);

#endif