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
#include "headers/utility.h"

#define PROJECTILESPEED 25.0f

#define ENEMYSPEED 3.0f
#define ENEMYMAXPDISTANCE 350
#define ENEMYMINPDISTANCE 250
#define HEALTHBAROFFSETY -10

// local functions
void EnemyPop(EnemyLL *prePop, EnemyLL **toPop);

void EnemyPop(EnemyLL *prePop, EnemyLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
}

int SpawnEnemies(GameDataS *gameData, int number, EnemyType enemyType)
{
    int i;
    int ret = 0;

    for(i = 0; i < number; i++)
    {
        ret = SpawnEnemy(gameData, enemyType);
        if(ret) return ret;
    }
    return ret;
}

int SpawnEnemy(GameDataS *gameData, float x, float y, EnemyType enemyType)
{
    EnemyLL *aux1;
    EnemyLL *aux2;

    aux1 = (EnemyLL*)malloc(sizeof(EnemyLL));
    // if memory is allocated correctly
    if(aux1)
    {
        aux1->enemyType = enemyType;
        aux1->enemy = {
            x,
            y,
            gameData->enemiesList[enemyType].enemy.x,
            gameData->enemiesList[enemyType].enemy.y
        };
        aux1->behaviour = BACKING;
        aux1->hitPoint = gameData->enemiesList[enemyType].baseHealth;
        aux1->healthBar = { x, y+HEALTHBAROFFSETY, (float)aux1->hitPoint, 5 };
        aux2 = gameData->enemiesHead->next;
        gameData->enemiesHead->next = aux1;
        gameData->enemiesHead->next->next = aux2;
        return 0;
    }
    return -1;
}

int SpawnEnemy(GameDataS *gameData, EnemyType enemyType)
{
    EnemyLL *aux1;
    EnemyLL *aux2;
    float x, y;


    aux1 = (EnemyLL*)malloc(sizeof(EnemyLL));
    // if memory is allocated correctly
    if(aux1)
    {
        aux1->enemyType = enemyType;
        do{
            x = rand()%(WALLTHICKNESS*MAPX)-WALLTHICKNESS+WALLTHICKNESS;
            y = rand()%(WALLTHICKNESS*MAPY)-WALLTHICKNESS+WALLTHICKNESS;
            aux1->enemy = {
                x,
                y,
                gameData->enemiesList[enemyType].enemy.x,
                gameData->enemiesList[enemyType].enemy.y
            };
        }while(CheckHitboxMap(gameData->level->bitmap, aux1->enemy));

        aux1->behaviour = BACKING;
        aux1->hitPoint = gameData->enemiesList[enemyType].baseHealth;
        aux1->healthBar = { x, y+HEALTHBAROFFSETY, (float)aux1->hitPoint, 5 };
        aux2 = gameData->enemiesHead->next;
        gameData->enemiesHead->next = aux1;
        gameData->enemiesHead->next->next = aux2;
        return 0;
    }
    return -1;
}

void EnemiesShooting(EnemyLL *enemiesHead, ProjectileLL *projectileHead, EnemiesS enemiesList[], Rectangle *player)
{
    float Dx, Dy, tmp;
    ProjectileLL *aux1;
    ProjectileLL *aux2;
    EnemyLL *currentEnemy = enemiesHead;

    while(currentEnemy->next != NULL)
    {
        currentEnemy = currentEnemy->next;
        // tiro a caso se il nemico spara o no
        if(rand()%1000 < enemiesList[currentEnemy->enemyType].weapon.shotsDeelay)
        {
            aux1 = (ProjectileLL*)malloc(sizeof(ProjectileLL));
            if(aux1)
            {
                aux1->projectile = 
                {   currentEnemy->enemy.x,
                    currentEnemy->enemy.y,
                    enemiesList[currentEnemy->enemyType].weapon.projectileSize,
                    enemiesList[currentEnemy->enemyType].weapon.projectileSize
                };
                
                aux1->damage = enemiesList[currentEnemy->enemyType].weapon.damage;

                aux1->texture = &enemiesList[currentEnemy->enemyType].weapon.projectileTexture;

                Dx = aux1->projectile.x - player->x;
                Dy = aux1->projectile.y - player->y;

                tmp = abs(Dx) + abs(Dy);

                aux1->vX = enemiesList[currentEnemy->enemyType].weapon.projectileSpeed * (Dx / tmp);
                aux1->vY = enemiesList[currentEnemy->enemyType].weapon.projectileSpeed * (Dy / tmp);

                aux1->owner = ENEMY;

                aux2 = projectileHead->next;
                projectileHead->next = aux1;
                projectileHead->next->next = aux2;
            }   
        }
    }
}

void UpdateEnemies(EnemyLL *enemiesHead, Rectangle *player, int level[MAPY][MAPX])
{
    float Dx, Dy, tmp;
    EnemyLL *previousEnemy;
    EnemyLL *currentEnemy = enemiesHead;

    while(currentEnemy->next){

        previousEnemy = currentEnemy;
        currentEnemy = currentEnemy->next;

        if(0 >= currentEnemy->hitPoint)
        {
            EnemyPop(previousEnemy, &currentEnemy);
            continue;
        }
        if(STILL == currentEnemy->behaviour)
        {
            if(rand()%1000 < 970)
                continue;
            else
                currentEnemy->behaviour = BACKING;
        }
        else if(rand()%1000 < 5)
        {
            currentEnemy->behaviour = STILL;
            continue;
        }
        
        Dx = currentEnemy->enemy.x - player->x;
        Dy = currentEnemy->enemy.y - player->y;
        
        tmp = abs(Dx) + abs(Dy);
        
        if(abs(Dx)+abs(Dy) > ENEMYMAXPDISTANCE)
            currentEnemy->behaviour = APPROACHING;
        else if(abs(Dx)+abs(Dy) < ENEMYMINPDISTANCE)
            currentEnemy->behaviour = BACKING;

        Dx = ENEMYSPEED * (Dx / tmp);
        Dy = ENEMYSPEED * (Dy / tmp);

        switch(currentEnemy->behaviour)
        {
            case BACKING:
                currentEnemy->enemy.x += Dx;
                if (CheckHitboxMap(level, currentEnemy->enemy))
                    currentEnemy->enemy.x -= Dx;
                currentEnemy->enemy.y += Dy;
                if (CheckHitboxMap(level, currentEnemy->enemy))
                    currentEnemy->enemy.y -= Dy;
            break;
            case APPROACHING:
                currentEnemy->enemy.x -= Dx;
                if (CheckHitboxMap(level, currentEnemy->enemy))
                    currentEnemy->enemy.x += Dx;
                currentEnemy->enemy.y -= Dy;
                if (CheckHitboxMap(level, currentEnemy->enemy))
                    currentEnemy->enemy.y += Dy;
            break;
        }

        currentEnemy->healthBar.x = currentEnemy->enemy.x;
        currentEnemy->healthBar.y = currentEnemy->enemy.y+HEALTHBAROFFSETY;
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