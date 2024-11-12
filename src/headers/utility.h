// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef UTILITY_H
#define UTILITY_H

#include "raylib.h"
#include "global_types.h"

int CheckHitboxRec(Rectangle rect1, Rectangle rect2);
int CheckHitboxMap(int level[MAPY][MAPX], Rectangle *entity);
void SnapToMap(GameDataS *gameData, Rectangle *entity, Vector2 *lastPos);

#endif
