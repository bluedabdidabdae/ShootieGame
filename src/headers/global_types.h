// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <list>
#include <vector>

#include "raylib.h"

#define WALLTHICKNESS 35

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
typedef struct Enemy_Linked_List{
    Rectangle enemy;
    EnemyType enemyType;
    Behaviour behaviour;
    Rectangle healthBar;
    int hitPoint;
}EnemyL;
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
typedef enum projectile_owner_model{
    ENEMY,
    PLAYER
}ProjectileOwner;
//////////////////////////////////////////////////////////
typedef struct Projectile_linked_List{
    Rectangle projectile;
    Texture2D *texture; // will point to the loaded texture
    float vX;
    float vY;
    int damage;
    ProjectileOwner owner;
}ProjectileL;
//////////////////////////////////////////////////////////
typedef struct player_model{
    Rectangle player;
    bool isTexture;
    union{
        Texture2D playerTexture;
        Image playerImage;
    };
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
typedef struct settigns_flags_model{
    bool toggleFullscreen;
}SettingsFlags;
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// WAVE DATA
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
typedef struct wave_enemies_model{
    EnemyType enemyType;
    int nOfEnemies;
}WaveEnemiesL;
//////////////////////////////////////////////////////////
typedef struct wave_data_model{
    std::list<WaveEnemiesL> enemyList;
}WaveL;
//////////////////////////////////////////////////////////
typedef struct map_model{
    int sizeX;
    int sizeY;
    int **bitmap; // its a matrix
}MapS;
//////////////////////////////////////////////////////////
typedef struct level_data_model{
    bool isLevelLoaded;
    int levelId;
    std::list<WaveL> waveList;
    MapS map;
}LevelS;
//////////////////////////////////////////////////////////
typedef enum block_type_model{
    NOT_LOADED,
    SPECIAL_NO_HITB,
    WALL_NO_HITB,
    FLOOR_NO_HITB,

    HITBOX_START,

    SPECIAL_HITB,
    WALL_HITB,
    FLOOR_HITB
}BlockType;
//////////////////////////////////////////////////////////
typedef struct custom_texture_2d_model{
    Texture2D texture;
    BlockType blockType;
}CustomTexture2D;
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// COMPLETE GAME DATA STRUCT
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
typedef struct game_data_model{
    // data relative to the actual game
    uint frameCounter;
    uint score;
    Camera2D camera;
    PlayerS player;
    std::list<EnemyL> enemiesList;
    std::list<ProjectileL> projectileList;
    std::vector<CustomTexture2D> mapTextures;
    WeaponS *weaponsList; // its an array
    EnemiesS *enemiesTemplateList; // its an array
    Vector2 mousePosition;
    LevelS level;
}GameDataS;
//////////////////////////////////////////////////////////
typedef struct app_data_model{
    // data relative to the whole program
    ToDraw toDraw;
    SettingsFlags settingsFlags;
    GameDataS *gameData; // will be allocated once the game starts
}AppDataS;
//////////////////////////////////////////////////////////

#endif