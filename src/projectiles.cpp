// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <time.h> // FOR WINDOWS

#include "raylib.h"
#include "headers/projectiles.h"

void UpdateProjectiles(ProjectileLL *projectileHead)
{
    while(projectileHead->next != NULL)
    {
        projectileHead = projectileHead->next;
        projectileHead->projectile.x -= projectileHead->vX;
        projectileHead->projectile.y -= projectileHead->vY;
    }
}

void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
    prePop = NULL;
}

void CheckProjectilesBorders(ProjectileLL *currentProjectile, Rectangle mapBorder[])
{
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
            if(currentProjectile->projectile.x < mapBorder[1].x+WALLTHICKNESS)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.x > mapBorder[3].x-currentProjectile->projectile.width)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.y < mapBorder[0].y+WALLTHICKNESS)
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