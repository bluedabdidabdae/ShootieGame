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
}States;
//////////////////////////////////////////////////////////
typedef enum behaviour_model{
    APPROACHING,
    BACKING,
    STILL
}Behaviour;
//////////////////////////////////////////////////////////
typedef enum enemy_type_model{
    MINION,
    SNIPER
}EnemyType;
//////////////////////////////////////////////////////////
typedef struct Weapon_model{
    char weaponName[WEAPONNAMEBUFFER];
    int damage;
    int shotsDeelay;
    float projectileSize;
    int projectileSpeed;
    bool isTexture;
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
    bool isTexture;
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
    float speed;
    // weapon variables
    int weapons[2];
    int activeWeaponId;
    // dodge variables
    float dodgeSpeed;
    int nextDodgeFrame;
    int dodgeDeelayFrames;
    int dodgeEndFrame;
    int dodgeDurationFrames;
    struct{
        bool isWalking;
        bool canDodge;
        bool isDodging;
        bool dodgeInvulnFrame;
        bool isInvulnerable;
        bool isStunned;
    }flags;
}PlayerS;
//////////////////////////////////////////////////////////
typedef struct game_skin_model{
    Color primaryColor;
    Color secondaryColor;
}GameSkinS;
//////////////////////////////////////////////////////////
typedef enum to_draw_status{
    DRAW_WAIT,
    DRAWMAINMENU,
    DRAWGAME,
    DRAWSETTINGS,
    DRAWCLOSEGAME,
    DRAW_LOAD_TEXTURES,
    DRAW_UNLOAD_TEXTURES,
    DRAWABORT
}ToDraw;
//////////////////////////////////////////////////////////
typedef struct settigns_flags_model{
    bool toggleFullscreen;
}SettingsFlags;
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// LEVEL HANDLING DATA
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
typedef struct wave_enemies_model{
    EnemyType enemyType;
    int nOfEnemies;
    wave_enemies_model *next;
}WaveEnemiesLL;
//////////////////////////////////////////////////////////
typedef struct wave_data_model{
    WaveEnemiesLL enemies;
    wave_data_model *next;
}WaveLL;
//////////////////////////////////////////////////////////
typedef struct level_data_model{
    int bitmap[MAPX][MAPY];
    WaveLL currentWave;
}LevelS;
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// COMPLETE GAME DATA STRUCT
//////////////////////////////////////////////////////////
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
    LevelS level;
    uint score;
    SettingsFlags settingsFlags;
}GameDataS;
//////////////////////////////////////////////////////////

#endif