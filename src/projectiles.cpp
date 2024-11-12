// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <time.h> // FOR WINDOWS

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

    while(currentProjectile->next != NULL)
    {
        previousProjectile = currentProjectile;
        currentProjectile = currentProjectile->next;
        switch(currentProjectile->owner)
        {
            case ENEMY:
                if(CheckHitboxRec(currentProjectile->projectile, gameData->player->player))
                {
                    (*gameData->player).lives -= currentProjectile->damage;
                    ProjectilePop(previousProjectile, &currentProjectile);
                }
            break;
            case PLAYER:
                currentEnemy = gameData->enemiesHead;
                while(currentEnemy->next != NULL)
                {
                    currentEnemy = currentEnemy->next;
                    if(CheckHitboxRec(currentProjectile->projectile, currentEnemy->enemy))
                    {
                        currentEnemy->hitPoint -= currentProjectile->damage;
                        ProjectilePop(previousProjectile, &currentProjectile);
                    }
                }
            break;
        }
    }
}

void UpdateProjectiles(ProjectileLL *projectileHead)
{
    ProjectileLL *currentProjectile = projectileHead;
    while(currentProjectile->next != NULL)
    {
        currentProjectile = currentProjectile->next;
        currentProjectile->projectile.x -= currentProjectile->vX;
        currentProjectile->projectile.y -= currentProjectile->vY;
    }
}

void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
    prePop = NULL;
}

void CheckProjectilesBorders(ProjectileLL *projectileHead, int level[MAPY][MAPX])
{
    ProjectileLL *currentProjectile = projectileHead;
    ProjectileLL *previousProjectile;

    if(currentProjectile->next)
    {
        do{
            // passo al prossimo proiettile
            previousProjectile = currentProjectile;
            currentProjectile = currentProjectile->next;

            // if it collides with the map i explode it
            if(CheckHitboxMap(level, &currentProjectile->projectile))
                ProjectilePop(previousProjectile, &currentProjectile);

            // check se ho fatto il pop dell'ultimo elemento della lista
            if(currentProjectile == NULL)
                return;
            
            // check se ho finito la lista
        }while(currentProjectile->next != NULL);
    }
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