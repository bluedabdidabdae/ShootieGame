// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <math.h>
#include <stdlib.h>
#include <iterator>
#include <list>
#include <vector>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/utility.h"
#include "headers/projectiles.h"
#include "headers/utility.h"

void CheckProjEntityDamage(std::list<ProjectileL> &projectileList, std::list<EnemyL> &enemiesList, PlayerS &player)
{
    std::list<ProjectileL>::iterator projectileIter = (projectileList.begin());
    while(projectileList.end() != projectileIter)
    {
        switch(projectileIter->owner)
        {
            case ENEMY:
                if(!player.flags.isInvulnerable && CheckHitboxRec(projectileIter->projectile, player.player))
                {
                    player.lives -= projectileIter->damage;
                    projectileIter = projectileList.erase(projectileIter);
                    projectileIter--;
                }
            break;
            case PLAYER:
                std::list<EnemyL>::iterator currentEnemy = enemiesList.begin();
                while(enemiesList.end() != currentEnemy)
                {
                    if(CheckHitboxRec(projectileIter->projectile, currentEnemy->enemy))
                    {
                        currentEnemy->hitPoint -= projectileIter->damage;
                        projectileIter = projectileList.erase(projectileIter);
                        projectileIter--;
                        break;
                        // this break IS NECESSARY
                    }
                    currentEnemy++;
                }
            break;
        }
        projectileIter++;
    }
}

void UpdateProjectiles(std::list<ProjectileL> &projectileList, LevelS &level, std::vector<CustomTexture2D> &blockList)
{
    std::list<ProjectileL>::iterator projectileIter = projectileList.begin();
    while(projectileIter != projectileList.end())
    {
        projectileIter->projectile.x -= projectileIter->vX;
        projectileIter->projectile.y -= projectileIter->vY;
        if(CheckHitboxMap(projectileIter->projectile, level.map, blockList))
            projectileIter = projectileList.erase(projectileIter);
        else
            projectileIter++;
    }
}