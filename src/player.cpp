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
    float Dx, Dy, tmp;
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

        Dx = aux1->projectile.x - mousePosition->x;
        Dy = aux1->projectile.y - mousePosition->y;

        tmp = abs(Dx) + abs(Dy);

        aux1->vX = weaponsList[player->activeWeaponId].projectileSpeed * (Dx / tmp);
        aux1->vY = weaponsList[player->activeWeaponId].projectileSpeed * (Dy / tmp);
        
        aux1->owner = PLAYER;

        aux2 = projectileHead->next;
        projectileHead->next = aux1;
        projectileHead->next->next = aux2;
    }
}

void UpdatePlayer(PlayerS *player, int level[MAPY][MAPX])
{
    int x = player->player.x/WALLTHICKNESS;
    int y = player->player.y/WALLTHICKNESS;

    if (IsKeyDown(KEY_W))
    {
        player->player.y -= PLAYERSPEED;
        if (CheckHitboxMap(level, &player->player))
            player->player.y = WALLTHICKNESS*(int)(player->player.y/WALLTHICKNESS)+WALLTHICKNESS;
    }
    if (IsKeyDown(KEY_A))
    {
        player->player.x -= PLAYERSPEED;
        if (CheckHitboxMap(level, &player->player))
            player->player.x = WALLTHICKNESS*(int)(player->player.x/WALLTHICKNESS)+WALLTHICKNESS;
    }
    if (IsKeyDown(KEY_S))
    {
        player->player.y += PLAYERSPEED;
        if (CheckHitboxMap(level, &player->player))
        {
            player->player.y = WALLTHICKNESS*(int)(player->player.y/WALLTHICKNESS);
            player->player.y += WALLTHICKNESS-player->player.height-1;
        }
    }
    if (IsKeyDown(KEY_D))
    {
        player->player.x += PLAYERSPEED;
        if (CheckHitboxMap(level, &player->player))
        {
            player->player.x = WALLTHICKNESS*(int)(player->player.x/WALLTHICKNESS);
            player->player.x += WALLTHICKNESS-player->player.width-1;
        }
    }
}