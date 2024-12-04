// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <math.h>
#include <stdlib.h>
#include <iterator>
#include <list>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/utility.h"
#include "headers/projectiles.h"
#include "headers/utility.h"

void CheckProjEntityDamage(GameDataS &gameData)
{
    std::list<ProjectileL>::iterator currentProjectile = gameData.projectileList.begin();
    while(gameData.projectileList.end() != currentProjectile)
    {
        switch(currentProjectile->owner)
        {
            case ENEMY:
                if(!gameData.player->flags.isInvulnerable && CheckHitboxRec(currentProjectile->projectile, gameData.player->player))
                {
                    gameData.player->lives -= currentProjectile->damage;
                    currentProjectile = gameData.projectileList.erase(currentProjectile);
                    currentProjectile--;
                }
            break;
            case PLAYER:
                std::list<EnemyL>::iterator currentEnemy = gameData.enemiesList.begin();
                while(gameData.enemiesList.end() != currentEnemy)
                {
                    if(CheckHitboxRec(currentProjectile->projectile, currentEnemy->enemy))
                    {
                        currentEnemy->hitPoint -= currentProjectile->damage;
                        currentProjectile = gameData.projectileList.erase(currentProjectile);
                        currentProjectile--;
                        break;
                        // this break IS NECESSARY
                    }
                    currentEnemy++;
                }
            break;
        }
        currentProjectile++;
    }
}

void UpdateProjectiles(std::list<ProjectileL> &projectileList, int level[MAPY][MAPX])
{
    std::list<ProjectileL>::iterator currentProjectile = projectileList.begin();
    while(currentProjectile != projectileList.end())
    {
        currentProjectile->projectile.x -= currentProjectile->vX;
        currentProjectile->projectile.y -= currentProjectile->vY;
        if(CheckHitboxMap(level, currentProjectile->projectile))
            currentProjectile = projectileList.erase(currentProjectile);
        else
            currentProjectile++;
    }
}