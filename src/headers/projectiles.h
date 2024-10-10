// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PROJECTILES_H
#define PROJECTILES_H

#define PROJECTILESPEED 12.0f
#define PLAYERPROJECTILESPEED 17.0f

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

void UpdateProjectiles(ProjectileLL *projectileHead);
void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop);
void CheckProjectilesBorders(ProjectileLL *projectileHead, Rectangle mapBorder[]);
void CompletelyDeleteAllProjectiles(ProjectileLL *head);

#endif