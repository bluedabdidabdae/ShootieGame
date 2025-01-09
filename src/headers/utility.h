// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef UTILITY_H
#define UTILITY_H

#include "raylib.h"
#include "global_types.h"

#define CHECK_HITBOX_REC(rect1, rect2) \
    rect1.x < rect2.x + rect2.width && \
    rect1.y < rect2.y + rect2.height && \
    rect1.x > rect2.x - rect2.width && \
    rect1.y > rect2.y - rect2.height

int CheckHitboxRec(Rectangle rect1, Rectangle rect2);
int CheckHitboxMap(Rectangle entity, MapS &map, std::vector<CustomTexture2D> &mapTextures);

#endif
