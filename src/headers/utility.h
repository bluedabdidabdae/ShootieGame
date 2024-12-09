// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef UTILITY_H
#define UTILITY_H

#include "raylib.h"
#include "global_types.h"

int CheckHitboxRec(Rectangle rect1, Rectangle rect2);
int CheckHitboxMap(Rectangle entity, int **level, int sizeX, int sizeY);

#endif
