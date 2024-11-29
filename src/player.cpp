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

void PlayerShooting(PlayerS *player, WeaponS weaponsList[], ProjectileLL *projectileHead, Vector2 *mousePosition)
{
    float dX, dY, tmp;
    ProjectileLL *aux1;
    ProjectileLL *aux2;

    aux1 = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    if(aux1)
    {
        aux1->projectile =
        {
            player->player.x,
            player->player.y,
            weaponsList[player->activeWeaponId].projectileSize,
            weaponsList[player->activeWeaponId].projectileSize
        };
        
        aux1->damage = weaponsList[player->activeWeaponId].damage;

        aux1->texture = &weaponsList[player->activeWeaponId].projectileTexture;

        dX = aux1->projectile.x - mousePosition->x;
        dY = aux1->projectile.y - mousePosition->y;

        tmp = abs(dX) + abs(dY);

        aux1->vX = weaponsList[player->activeWeaponId].projectileSpeed * (dX / tmp);
        aux1->vY = weaponsList[player->activeWeaponId].projectileSpeed * (dY / tmp);
        
        aux1->owner = PLAYER;

        aux2 = projectileHead->next;
        projectileHead->next = aux1;
        projectileHead->next->next = aux2;
    }
}

void UpdatePlayer(PlayerS *player, int level[MAPY][MAPX], uint currentFrame)
{
    float tmp;
    float dX = 0;
    float dY = 0;

    if(!player->flags.isStunned && currentFrame >= player->nextDodgeFrame && IsKeyDown(KEY_SPACE))
    {
        player->flags.isDodging = true;
        
        if(player->flags.dodgeInvulnFrame)
            player->flags.isInvulnerable = true;

        player->nextDodgeFrame = currentFrame + player->dodgeDeelayFrames;
        player->dodgeEndFrame = currentFrame + player->dodgeDurationFrames;
    }
    if(player->flags.isDodging && (player->flags.isStunned || currentFrame >= player->dodgeEndFrame))
    {
        player->flags.isDodging = false;
        player->flags.isInvulnerable = false;
    }

    if (IsKeyDown(KEY_W))
    {
        if(player->flags.isDodging)
            player->player.y -= player->dodgeSpeed;
        else
            player->player.y -= player->speed;
        if (CheckHitboxMap(level, &player->player))
        player->player.y = WALLTHICKNESS*(int)(player->player.y/WALLTHICKNESS)+WALLTHICKNESS;
    }

    if (IsKeyDown(KEY_A))
    {
        if(player->flags.isDodging)
            player->player.x -= player->dodgeSpeed;
        else
            player->player.x -= player->speed;
        if (CheckHitboxMap(level, &player->player))
        player->player.x = WALLTHICKNESS*(int)(player->player.x/WALLTHICKNESS)+WALLTHICKNESS;
    }

    if (IsKeyDown(KEY_S))
    {
        if(player->flags.isDodging)
            player->player.y += player->dodgeSpeed;
        else
            player->player.y += player->speed;
        if (CheckHitboxMap(level, &player->player))
        {
            player->player.y = WALLTHICKNESS*(int)(player->player.y/WALLTHICKNESS);
            player->player.y += WALLTHICKNESS-player->player.height-1;
        }
    }

    if (IsKeyDown(KEY_D))
    {
        if(player->flags.isDodging)
            player->player.x += player->dodgeSpeed;
        else
            player->player.x += player->speed;
        if (CheckHitboxMap(level, &player->player))
        {
            player->player.x = WALLTHICKNESS*(int)(player->player.x/WALLTHICKNESS);
            player->player.x += WALLTHICKNESS-player->player.width-1;
        }
    }
}