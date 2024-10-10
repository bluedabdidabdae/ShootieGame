// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GRAPHIC_H
#define GRAPHIC_H

#define WIDTH 1335
#define HEIGT 675
#define WINDOWNAME "Shootie Shootie Game"
#define TARGETFPS 60
#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define FADEVALUE 0.2
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270
#define MAINMENUTEXTCOLOR RAYWHITE
#define WALLTHICKNESS 10

#include "projectiles.h"
#include "enemies.h"

//////////////////////////////////////////////////////////
typedef enum to_draw_status{
    MAINMENU,
    GAME,
    CLOSEGAME
}ToDraw;

typedef struct game_data_model{
    ToDraw *toDraw;
    uint frameCounter;
    Vector2 *mousePosition;
    Camera2D *camera;
    Rectangle *player;
    Rectangle *mapBorder;
    EnemyLL *enemiesHead;
    ProjectileLL *projectileHead;
}GameDataS;
//////////////////////////////////////////////////////////

void *HandleGraphics(void* data);
void DrawMenu();
void DrawGame(void* data);

#endif