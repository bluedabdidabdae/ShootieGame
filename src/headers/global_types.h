// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "raylib.h"
#include "projectiles.h"
#include "enemies.h"

//////////////////////////////////////////////////////////
typedef unsigned int uint;
//////////////////////////////////////////////////////////
typedef enum StatesModel{
    MENU,       // 0
    PLAY,       // 1
    SETTINGS,   // 2
    EXITGAME,   // 3
    MEMERR      // 4
} States;
//////////////////////////////////////////////////////////
typedef enum to_draw_status{
    MAINMENU,
    GAME,
    CLOSEGAME
}ToDraw;

typedef struct game_data_model{
    ToDraw toDraw;
    Vector2 mousePosition;
    Camera2D camera;
    Rectangle player;
    Rectangle mapBorder[4];
    EnemyLL *enemiesHead;
    ProjectileLL *projectileHead;
}GameDataS;
//////////////////////////////////////////////////////////
#define WIDTH 1335
#define HEIGT 675
#define WALLTHICKNESS 15.0f

#endif