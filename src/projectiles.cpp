// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/utility.h"
#include "headers/projectiles.h"
#include "headers/utility.h"

// local functions
void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop);

void CheckProjEntityDamage(GameDataS *gameData)
{
    ProjectileLL *currentProjectile = gameData->projectileHead;
    ProjectileLL *previousProjectile;
    EnemyLL *currentEnemy;

    while(currentProjectile->next)
    {
        previousProjectile = currentProjectile;
        currentProjectile = currentProjectile->next;

        switch(currentProjectile->owner)
        {
            case ENEMY:
                if(gameData->player->flags.isInvulnerable)
                    continue;
                if(CheckHitboxRec(currentProjectile->projectile, gameData->player->player))
                {
                    gameData->player->lives -= currentProjectile->damage;
                    ProjectilePop(previousProjectile, &currentProjectile);
                }
            break;
            case PLAYER:
                currentEnemy = gameData->enemiesHead;
                while(currentEnemy->next)
                {
                    currentEnemy = currentEnemy->next;
                    if(CheckHitboxRec(currentProjectile->projectile, currentEnemy->enemy))
                    {
                        currentEnemy->hitPoint -= currentProjectile->damage;
                        ProjectilePop(previousProjectile, &currentProjectile);
                        break;
                        // this break IS NECESSARY
                    }
                }
            break;
        }
    }
}

void UpdateProjectiles(ProjectileLL *projectileHead, int level[MAPY][MAPX])
{
    ProjectileLL *currentProjectile = projectileHead;
    ProjectileLL *previousProjectile;

    while(currentProjectile->next){
        // passo al prossimo proiettile
        previousProjectile = currentProjectile;
        currentProjectile = currentProjectile->next;

        currentProjectile->projectile.x -= currentProjectile->vX;
        currentProjectile->projectile.y -= currentProjectile->vY;

        // if it collides with the map i explode it
        if(CheckHitboxMap(level, currentProjectile->projectile))
        {
            ProjectilePop(previousProjectile, &currentProjectile);
            
        }
    }
}

void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
}

void CompletelyDeleteAllProjectiles(ProjectileLL *head)
{
    ProjectileLL *tmp;
    while(head->next != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    free(head);
}