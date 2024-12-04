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

int SpawnEnemies(int number, EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, int level[MAPY][MAPX])
{
    int i;
    int ret = 0;

    for(i = 0; i < number; i++)
    {
        ret = SpawnEnemy(enemyType, enemiesList, enemiesTemplateList, level);
        if(ret) return ret;
    }
    return ret;
}

int SpawnEnemiesPos(int number, EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList)
{
    int i;
    int ret = 0;

    for(i = 0; i < number; i++)
    {
        ret = SpawnEnemyPos(enemyType, x, y, enemiesList, enemiesTemplateList);
        if(ret) return ret;
    }
    return ret;
}

int SpawnEnemy(EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, int level[MAPY][MAPX])
{
    float x, y;

    EnemyL tmpEnemy;

    do{
        x = rand()%(WALLTHICKNESS*MAPX)-WALLTHICKNESS+WALLTHICKNESS;
        y = rand()%(WALLTHICKNESS*MAPY)-WALLTHICKNESS+WALLTHICKNESS;
        tmpEnemy.enemy = {
            x,
            y,
            enemiesTemplateList[enemyType].enemy.x,
            enemiesTemplateList[enemyType].enemy.y
        };
    }while(CheckHitboxMap(level, tmpEnemy.enemy));

    tmpEnemy.enemyType = enemyType;
    tmpEnemy.behaviour = BACKING;
    tmpEnemy.hitPoint = enemiesTemplateList[enemyType].baseHealth;
    tmpEnemy.healthBar = { x, y+HEALTHBAROFFSETY, (float)tmpEnemy.hitPoint, 5 };
    
    enemiesList.push_front(tmpEnemy);

    return 0;
}

int SpawnEnemyPos(EnemyType enemyType, float x, float y, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList)
{
    EnemyL tmpEnemy;

    tmpEnemy.enemy = {
        x,
        y,
        enemiesTemplateList[enemyType].enemy.x,
        enemiesTemplateList[enemyType].enemy.y
    };
    tmpEnemy.enemyType = enemyType;
    tmpEnemy.behaviour = BACKING;
    tmpEnemy.hitPoint = enemiesTemplateList[enemyType].baseHealth;
    tmpEnemy.healthBar = { x, y+HEALTHBAROFFSETY, (float)tmpEnemy.hitPoint, 5 };
    
    enemiesList.push_front(tmpEnemy);

    return 0;
}

void EnemiesShooting(std::list<EnemyL> &enemiesList, std::list<ProjectileL> &projectileList, EnemiesS enemiesTemplateList[], Rectangle *player)
{
    float Dx, Dy, tmp;
    ProjectileL tmpProjectile;

    std::list<EnemyL>::iterator currentEnemy = enemiesList.begin();
    while(enemiesList.end() != currentEnemy)
    {
        // tiro a caso se il nemico spara o no
        if(rand()%1000 < enemiesTemplateList[currentEnemy->enemyType].weapon.shotsDeelay)
        {
            tmpProjectile = 
            {   currentEnemy->enemy.x,
                currentEnemy->enemy.y,
                enemiesTemplateList[currentEnemy->enemyType].weapon.projectileSize,
                enemiesTemplateList[currentEnemy->enemyType].weapon.projectileSize
            };
            
            tmpProjectile.damage = enemiesTemplateList[currentEnemy->enemyType].weapon.damage;

            tmpProjectile.texture = &enemiesTemplateList[currentEnemy->enemyType].weapon.projectileTexture;

            Dx = tmpProjectile.projectile.x - player->x;
            Dy = tmpProjectile.projectile.y - player->y;

            tmp = abs(Dx) + abs(Dy);

            tmpProjectile.vX = enemiesTemplateList[currentEnemy->enemyType].weapon.projectileSpeed * (Dx / tmp);
            tmpProjectile.vY = enemiesTemplateList[currentEnemy->enemyType].weapon.projectileSpeed * (Dy / tmp);

            tmpProjectile.owner = ENEMY;

            projectileList.push_front(tmpProjectile);
            
        }
        currentEnemy++;
    }
}

void UpdateEnemies(std::list<EnemyL> &enemiesList, Rectangle *player, int level[MAPY][MAPX])
{
    float Dx, Dy, tmp;

    std::list<EnemyL>::iterator currentEnemy = enemiesList.begin();
    while(enemiesList.end() != currentEnemy){

        if(0 >= currentEnemy->hitPoint)
        {
            currentEnemy = enemiesList.erase(currentEnemy);
            continue;
        }
        if(STILL == currentEnemy->behaviour)
        {
            if(rand()%1000 < 970)
            {
                currentEnemy++;
                continue;
            }
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

        currentEnemy++;
    }
}