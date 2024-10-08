// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "global_types.h"
#include "projectiles.h"
#include "player.h"
#include "enemies.h"
#include "graphic.h"
#include "game_engine.h"

#define WIDTH 1335
#define HEIGT 675
#define WINDOWNAME "Shootie Shootie Game"
#define TARGETFPS 60

void *HandleGraphics(void* data);
void DrawMenu();
#endif