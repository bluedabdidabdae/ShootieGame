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

int SpawnEnemies(int number, EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, LevelS &level)
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

int SpawnEnemy(EnemyType enemyType, std::list<EnemyL> &enemiesList, EnemiesS *enemiesTemplateList, LevelS &level)
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
    }while(CheckHitboxMap(tmpEnemy.enemy, level.map));

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

void EnemiesShooting(std::list<EnemyL> &enemiesList, std::list<ProjectileL> &projectileList, EnemiesS enemiesTemplateList[], Rectangle &player)
{
    float Dx, Dy, tmp;
    ProjectileL tmpProjectile;

    std::list<EnemyL>::iterator enemyIter = enemiesList.begin();
    while(enemiesList.end() != enemyIter)
    {
        // tiro a caso se il nemico spara o no
        if(rand()%1000 < enemiesTemplateList[enemyIter->enemyType].weapon.shotsDeelay)
        {
            tmpProjectile = 
            {   enemyIter->enemy.x,
                enemyIter->enemy.y,
                enemiesTemplateList[enemyIter->enemyType].weapon.projectileSize,
                enemiesTemplateList[enemyIter->enemyType].weapon.projectileSize
            };
            
            tmpProjectile.damage = enemiesTemplateList[enemyIter->enemyType].weapon.damage;

            tmpProjectile.texture = &enemiesTemplateList[enemyIter->enemyType].weapon.projectileTexture;

            Dx = tmpProjectile.projectile.x - player.x;
            Dy = tmpProjectile.projectile.y - player.y;

            tmp = abs(Dx) + abs(Dy);

            tmpProjectile.vX = enemiesTemplateList[enemyIter->enemyType].weapon.projectileSpeed * (Dx / tmp);
            tmpProjectile.vY = enemiesTemplateList[enemyIter->enemyType].weapon.projectileSpeed * (Dy / tmp);

            tmpProjectile.owner = ENEMY;

            projectileList.push_front(tmpProjectile);
            
        }
        enemyIter++;
    }
}

void UpdateEnemies(std::list<EnemyL> &enemiesList, Rectangle &player, MapS &map)
{
    float Dx, Dy, tmp;

    std::list<EnemyL>::iterator enemyIter = enemiesList.begin();
    while(enemiesList.end() != enemyIter){

        if(0 >= enemyIter->hitPoint)
        {
            enemyIter = enemiesList.erase(enemyIter);
            continue;
        }
        if(STILL == enemyIter->behaviour)
        {
            if(rand()%1000 < 970)
            {
                enemyIter++;
                continue;
            }
            else
                enemyIter->behaviour = BACKING;
        }
        else if(rand()%1000 < 5)
        {
            enemyIter->behaviour = STILL;
            continue;
        }
        
        Dx = enemyIter->enemy.x - player.x;
        Dy = enemyIter->enemy.y - player.y;
        
        tmp = abs(Dx) + abs(Dy);
        
        if(abs(Dx)+abs(Dy) > ENEMYMAXPDISTANCE)
            enemyIter->behaviour = APPROACHING;
        else if(abs(Dx)+abs(Dy) < ENEMYMINPDISTANCE)
            enemyIter->behaviour = BACKING;

        Dx = ENEMYSPEED * (Dx / tmp);
        Dy = ENEMYSPEED * (Dy / tmp);

        switch(enemyIter->behaviour)
        {
            case BACKING:
                enemyIter->enemy.x += Dx;
                if (CheckHitboxMap(enemyIter->enemy, map))
                    enemyIter->enemy.x -= Dx;
                enemyIter->enemy.y += Dy;
                if (CheckHitboxMap(enemyIter->enemy, map))
                    enemyIter->enemy.y -= Dy;
            break;
            case APPROACHING:
                enemyIter->enemy.x -= Dx;
                if (CheckHitboxMap(enemyIter->enemy, map))
                    enemyIter->enemy.x += Dx;
                enemyIter->enemy.y -= Dy;
                if (CheckHitboxMap(enemyIter->enemy, map))
                    enemyIter->enemy.y += Dy;
            break;
        }

        enemyIter->healthBar.x = enemyIter->enemy.x;
        enemyIter->healthBar.y = enemyIter->enemy.y+HEALTHBAROFFSETY;

        enemyIter++;
    }
}