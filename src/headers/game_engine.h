// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#define WIDTH 1335
#define HEIGT 675
#define WALLTHICKNESS 15.0f
#define ENEMYSPEED 3.0f
#define ENEMYMAXPDISTANCE 500
#define ENEMYMINPDISTANCE 400
#define PLAYERSPEED 5.0f
#define PROJECTILESPEED 12.0f

typedef unsigned int uint;

typedef enum behaviour_model{
        APPROACHING,
        BACKING,
        STILL
}Behaviour;

typedef struct Enemy_Linked_List{
    Behaviour behaviour;
    Rectangle enemy;
    Color color;
    Rectangle healthBar;
    int hitPoint;
    Enemy_Linked_List *nextEnemy;
}EnemyLL;

typedef struct Projectile_linked_List{
    Rectangle projectile;
    Color color;
    float vX;
    float vY;
    Projectile_linked_List *nextProjectile;
}ProjectileLL;

int GameEngine();
void DrawGame(Camera2D *camera, EnemyLL *currentEnemy, Rectangle *player, Rectangle mapBorder[], ProjectileLL *projectileHead);
void UpdatePlayer(Rectangle *player);
int SpawnEnemy(EnemyLL **destination, float x, float y);
void DeleteEnemies(EnemyLL *head);
void UpdateTrackingEntity(EnemyLL *currentEnemy, Rectangle *player);
void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player);
void EnemiesShooting(EnemyLL *currentEnemy, uint frameCounter, ProjectileLL *projectileHead, Rectangle *player);
void UpdateProjectiles(ProjectileLL *projectileHead);
void DeleteProjectiles(ProjectileLL *head);
void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[]);
void CheckProjectilesBorders(ProjectileLL *currentProjectile, Rectangle mapBorder[]);
void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop);