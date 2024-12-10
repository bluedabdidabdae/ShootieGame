// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctime> // FOR LINUX
// #include <time.h> // FOR WINDOWS

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/player.h"
#include "headers/utility.h"

void PlayerShooting(PlayerS &player, WeaponS weaponsList[], std::list<ProjectileL> &projectileList, Vector2 &mousePosition)
{
    float dX, dY, tmp;
    ProjectileL tmpProjectile;
    
    tmpProjectile.projectile =
    {
        player.player.x,
        player.player.y,
        weaponsList[player.activeWeaponId].projectileSize,
        weaponsList[player.activeWeaponId].projectileSize
    };
    
    tmpProjectile.damage = weaponsList[player.activeWeaponId].damage;

    tmpProjectile.texture = &weaponsList[player.activeWeaponId].projectileTexture;

    dX = tmpProjectile.projectile.x - mousePosition.x;
    dY = tmpProjectile.projectile.y - mousePosition.y;

    tmp = abs(dX) + abs(dY);

    tmpProjectile.vX = weaponsList[player.activeWeaponId].projectileSpeed * (dX / tmp);
    tmpProjectile.vY = weaponsList[player.activeWeaponId].projectileSpeed * (dY / tmp);
    
    tmpProjectile.owner = PLAYER;

    projectileList.push_front(tmpProjectile);
}

void UpdatePlayer(PlayerS &player, LevelS &level, uint currentFrame)
{
    float tmp;
    float dX = 0;
    float dY = 0;
    int vX;
    int vY;

    if(player.flags.isStunned)
        return;

    if(currentFrame >= player.nextDodgeFrame && IsKeyDown(KEY_SPACE))
    {
        player.flags.isDodging = true;
        
        if(player.flags.dodgeInvulnFrame)
            player.flags.isInvulnerable = true;

        player.nextDodgeFrame = currentFrame + player.dodgeDeelayFrames;
        player.dodgeEndFrame = currentFrame + player.dodgeDurationFrames;
    }
    if(player.flags.isDodging && (player.flags.isStunned || currentFrame >= player.dodgeEndFrame))
    {
        player.flags.isDodging = false;
        player.flags.isInvulnerable = false;
    }

    if (IsKeyDown(KEY_W))
    {
        if(player.flags.isDodging)
            dY -= player.dodgeSpeed;
        else
            dY -= player.speed;
        if (CheckHitboxMap({player.player.x, player.player.y+dY,
                            player.player.width, player.player.height},
                            level.map))
        {
            player.player.y = WALLTHICKNESS*(int)(player.player.y/WALLTHICKNESS);
            dY = 0;
        }
    }

    if (IsKeyDown(KEY_A))
    {
        if(player.flags.isDodging)
            dX -= player.dodgeSpeed;
        else
            dX -= player.speed;
        if (CheckHitboxMap({player.player.x+dX, player.player.y,
                            player.player.width, player.player.height},
                            level.map))
        {
            player.player.x = WALLTHICKNESS*(int)(player.player.x/WALLTHICKNESS);
            dX = 0;
        }
    }

    if (IsKeyDown(KEY_S))
    {
        if(player.flags.isDodging)
            dY += player.dodgeSpeed;
        else
            dY += player.speed;
        if (CheckHitboxMap({player.player.x, player.player.y+dY,
                            player.player.width, player.player.height},
                            level.map))
        {
            player.player.y = WALLTHICKNESS*(int)(player.player.y/WALLTHICKNESS);
            player.player.y += WALLTHICKNESS-player.player.height;
            dY = 0;
        }
    }

    if (IsKeyDown(KEY_D))
    {
        if(player.flags.isDodging)
            dX += player.dodgeSpeed;
        else
            dX += player.speed;
        if (CheckHitboxMap({player.player.x+dX, player.player.y,
                            player.player.width, player.player.height},
                            level.map))
        {
            player.player.x = WALLTHICKNESS*(int)(player.player.x/WALLTHICKNESS);
            player.player.x += WALLTHICKNESS-player.player.width;
            dX = 0;
        }
    }

    // VI : VItot = VR : VRtot
    if(dX && dY)
    {
        dX /= 1.3;
        dY /= 1.3;
    }

    player.player.x += dX;
    player.player.y += dY;
}