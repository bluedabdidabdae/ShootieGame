// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctime> // FOR LINUX
// #include <time.h> // FOR WINDOWS

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/enemies.h"

#define PROJECTILESPEED 25.0f

#define ENEMYSPEED 3.0f
#define ENEMYMAXPDISTANCE 600
#define ENEMYMINPDISTANCE 500
#define HEALTHBAROFFSETY -10

// local functions
void EnemyPop(EnemyLL *prePop, EnemyLL **toPop);

void EnemyPop(EnemyLL *prePop, EnemyLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
    prePop = NULL;
}

int SpawnEnemy(EnemyLL *head, float x, float y)
{
    EnemyLL *aux1;
    EnemyLL *aux2;

    aux1 = (EnemyLL*)malloc(sizeof(EnemyLL));
    // if memory is allocated correctly
    if(aux1)
    {
        aux1->enemy = { x, y, 20, 20 };
        aux1->enemyType = NORMAL;
        aux1->color = BROWN;
        aux1->behaviour = BACKING;
        aux1->hitPoint = 20;
        aux1->healthBar = { x, y+HEALTHBAROFFSETY, 20, 5 };
        aux2 = head->next;
        head->next = aux1;
        head->next->next = aux2;
        return 0;
    }
    return -1;
}

void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[])
{
    while(currentEnemy->next != NULL){
        currentEnemy = currentEnemy->next;
        if(currentEnemy->enemy.x < mapBorder[1].x+mapBorder[1].width)
            currentEnemy->enemy.x = mapBorder[1].x+mapBorder[1].width;
        else if(currentEnemy->enemy.x > mapBorder[3].x-currentEnemy->enemy.width)
            currentEnemy->enemy.x = mapBorder[3].x-currentEnemy->enemy.width;
        
        if(currentEnemy->enemy.y < mapBorder[0].y+mapBorder[0].height)
            currentEnemy->enemy.y = mapBorder[0].y+mapBorder[0].height;
        else if(currentEnemy->enemy.y > mapBorder[2].y-currentEnemy->enemy.height)
            currentEnemy->enemy.y = mapBorder[2].y-currentEnemy->enemy.height;
    }
}

void EnemiesShooting(EnemyLL *currentEnemy, ProjectileLL *projectileHead, Rectangle *player)
{
    float Dx, Dy, tmp;
    ProjectileLL *aux1;
    ProjectileLL *aux2;

    while(currentEnemy->next != NULL)
    {
        currentEnemy = currentEnemy->next;
        // tiro a caso se il nemico spara o no
        if(rand()%1000 < 20)
        {
            // aggiungo un proiettile in coda alla lista e lo inizializzo
            // con le coordinate ed il valore dei vettori per poi aggiornarne
            // la posizione in "UpdateProjectiles(...)"
            aux1 = (ProjectileLL*)malloc(sizeof(ProjectileLL));
            if(aux1)
            {
                aux1->projectile = 
                {   currentEnemy->enemy.x+currentEnemy->enemy.width/2,
                    currentEnemy->enemy.y+currentEnemy->enemy.height/2,
                    5,
                    5
                };
                aux1->color = RED;

                Dx = aux1->projectile.x - (player->x + player->height / 2);
                Dy = aux1->projectile.y - (player->y + player->width / 2);

                tmp = abs(Dx) + abs(Dy);

                aux1->vX = PROJECTILESPEED * (Dx / tmp);
                aux1->vY = PROJECTILESPEED * (Dy / tmp);

                aux1->owner = ENEMY;

                aux2 = projectileHead->next;
                projectileHead->next = aux1;
                projectileHead->next->next = aux2;
            }   
        }
    }
}

void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player)
{
    float Dx, Dy, tmp;
    EnemyLL *previousEnemy;

    while(currentEnemy->next != NULL){

        previousEnemy = currentEnemy;
        currentEnemy = currentEnemy->next;

        if(currentEnemy->hitPoint <= 0)
        {
            EnemyPop(previousEnemy, &currentEnemy);
            goto ignore_stuff;
        }
        if(STILL == currentEnemy->behaviour)
        {
            if(rand()%1000 < 970)
                goto ignore_stuff;
            else
                currentEnemy->behaviour = BACKING;
        }
        else if(rand()%1000 < 5)
        {
            currentEnemy->behaviour = STILL;
            goto ignore_stuff;
        }
        
        Dx = currentEnemy->enemy.x - player->x;
        Dy = currentEnemy->enemy.y - player->y;
        
        tmp = abs(Dx) + abs(Dy);
        
        if(abs(Dx)+abs(Dy) > ENEMYMAXPDISTANCE)
            currentEnemy->behaviour = APPROACHING;
        else if(abs(Dx)+abs(Dy) < ENEMYMINPDISTANCE)
            currentEnemy->behaviour = BACKING;
        
        switch(currentEnemy->behaviour)
        {
            case BACKING:
                currentEnemy->enemy.x += ENEMYSPEED * (Dx / tmp);
                currentEnemy->enemy.y += ENEMYSPEED * (Dy / tmp);
            break;
            case APPROACHING:
                currentEnemy->enemy.x -= ENEMYSPEED * (Dx / tmp);
                currentEnemy->enemy.y -= ENEMYSPEED * (Dy / tmp);
            break;
        }
        
        currentEnemy->healthBar.x = currentEnemy->enemy.x;
        currentEnemy->healthBar.y = currentEnemy->enemy.y+HEALTHBAROFFSETY;
        
        ignore_stuff:
            continue;
    }
}

void CompletelyDeleteAllEnemies(EnemyLL *head)
{
    EnemyLL *tmp;
    while(head->next != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    free(head);
}