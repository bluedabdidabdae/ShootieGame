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
                if(CheckHitboxRec(currentProjectile->projectile, (*gameData->player).player))
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

void CheckProjectilesBorders(ProjectileLL *projectileHead, Rectangle mapBorder[])
{
    ProjectileLL *currentProjectile = currentProjectile;
    ProjectileLL *previousProjectile;

    if(currentProjectile->next != NULL)
    {
        do{
            // passo al prossimo proiettile (a differenza della
            // linked list dei nemici questa mantiene il primo 
            // nodo vuoto ed allocato fino a fine partita,
            // in futuro le faccio uguali, dipende
            // con quale dei due modi mi trovo più comodo)
            previousProjectile = currentProjectile;
            currentProjectile = currentProjectile->next;

            // deleto i proiettili che impattano coi bordi della mappa
            // LASCIARE TUTTI GLI ELSE, ALTRIMENTI SI SFONDA LA MEMORIA
            // controlli per le collisioni tra il proiettile ed i 4 bordi mappa
            if(currentProjectile->projectile.x < mapBorder[1].x+mapBorder[1].width)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.x > mapBorder[3].x-currentProjectile->projectile.width)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.y < mapBorder[0].y+mapBorder[0].height)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.y > mapBorder[2].y-currentProjectile->projectile.height)
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