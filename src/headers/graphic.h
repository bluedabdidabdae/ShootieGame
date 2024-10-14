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

#include "raylib.h"

//////////////////////////////////////////////////////////
typedef enum behaviour_model{
        APPROACHING,
        BACKING,
        STILL
}Behaviour;
typedef struct Enemy_Linked_List{
    Rectangle enemy;
    Color color;
    Behaviour behaviour;
    Rectangle healthBar;
    int hitPoint;
    Enemy_Linked_List *next;
}EnemyLL;
//////////////////////////////////////////////////////////
typedef enum projectile_owner_model{
    ENEMY,
    PLAYER
}ProjectileOwner;
//////////////////////////////////////////////////////////
typedef struct Projectile_linked_List{
    Rectangle projectile;
    Color color;
    float vX;
    float vY;
    ProjectileOwner owner;
    Projectile_linked_List *next;
}ProjectileLL;
//////////////////////////////////////////////////////////
typedef struct player_model{
    Rectangle player;
    int lives;
}PlayerS;

typedef struct game_skin_model{
    Color primaryColor;
    Color secondaryColor;
}GameSkinS;

typedef enum to_draw_status{
    MAINMENU,
    GAME,
    CLOSEGAME
}ToDraw;

typedef struct game_data_model{
    ToDraw *toDraw;
    GameSkinS *gameSkin;
    uint frameCounter;
    Vector2 *mousePosition;
    Camera2D *camera;
    PlayerS *player;
    Rectangle *mapBorder;
    EnemyLL *enemiesHead;
    ProjectileLL *projectileHead;
    uint score;
}GameDataS;
//////////////////////////////////////////////////////////

void *HandleGraphics(void* data);

#endif