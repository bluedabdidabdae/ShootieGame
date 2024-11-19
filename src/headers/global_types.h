// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "raylib.h"

#define WIDTH 1335
#define HEIGT 675
#define WALLTHICKNESS 30

#define WEAPONNAMEBUFFER 20
#define MAPX 53
#define MAPY 27

#define MALLOC_ERROR 11
#define THREAD_ERROR 12
#define FILE_ERROR 13
#define ARRAY_ERROR 14

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
typedef enum behaviour_model{
        APPROACHING,
        BACKING,
        STILL
}Behaviour;
//////////////////////////////////////////////////////////
typedef enum enemy_type_model{
    NORMAL
}EnemyType;
//////////////////////////////////////////////////////////
typedef struct Weapon_model{
    char weaponName[WEAPONNAMEBUFFER];
    int damage;
    int shotsDeelay;
    float projectileSize;
    int projectileSpeed;
    union{
        Texture2D projectileTexture;
        Image projectileImage;
    };
}WeaponS;
//////////////////////////////////////////////////////////
typedef struct Enemies_model{
    int baseHealth;
    int baseWeaponId;
    Vector2 enemy;
    WeaponS weapon;
    union{
        Texture2D enemyTexture;
        Image enemyImage;
    };
}EnemiesS;
//////////////////////////////////////////////////////////
typedef struct Enemy_Linked_List{
    Rectangle enemy;
    EnemyType enemyType;
    Behaviour behaviour;
    Rectangle healthBar;
    int hitPoint;
    Texture2D *texture;
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
    Texture2D *texture;
    float vX;
    float vY;
    int damage;
    ProjectileOwner owner;
    Projectile_linked_List *next;
}ProjectileLL;
//////////////////////////////////////////////////////////
typedef struct player_model{
    Rectangle player;
    int lives;
    int weapons[2];
    int activeWeaponId;
}PlayerS;
//////////////////////////////////////////////////////////
typedef struct game_skin_model{
    Color primaryColor;
    Color secondaryColor;
}GameSkinS;
//////////////////////////////////////////////////////////
typedef enum to_draw_status{
    MAINMENU,
    GAME,
    CLOSEGAME,
    ABORT
}ToDraw;
//////////////////////////////////////////////////////////
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
    WeaponS *weaponsList;
    EnemiesS *enemiesList;
    Texture2D *mapTextures;
    bool isCameraLocked;
    int level[MAPY][MAPX];
    uint score;
}GameDataS;
//////////////////////////////////////////////////////////

#endif