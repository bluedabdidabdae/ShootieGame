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

#define PLAYERPROJECTILESPEED 25.0f

void PlayerShooting(GameDataS *gameData)
{
    float Dx, Dy, tmp;
    ProjectileLL *aux1;
    ProjectileLL *aux2;

    aux1 = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    if(aux1)
    {
        aux1->projectile =
        {
            gameData->player->player.x + gameData->player->player.width / 2,
            gameData->player->player.y + gameData->player->player.height / 2,
            gameData->weaponsList[gameData->player->activeWeaponId].projectileSize,
            gameData->weaponsList[gameData->player->activeWeaponId].projectileSize
        };
        
        aux1->damage = gameData->weaponsList[gameData->player->activeWeaponId].damage;

        aux1->color = BLUE;

        Dx = aux1->projectile.x - gameData->mousePosition->x;
        Dy = aux1->projectile.y - gameData->mousePosition->y;

        tmp = abs(Dx) + abs(Dy);

        aux1->vX = gameData->weaponsList[gameData->player->activeWeaponId].projectileSpeed * (Dx / tmp);
        aux1->vY = gameData->weaponsList[gameData->player->activeWeaponId].projectileSpeed * (Dy / tmp);
        
        aux1->owner = PLAYER;

        aux2 = gameData->projectileHead->next;
        gameData->projectileHead->next = aux1;
        gameData->projectileHead->next->next = aux2;
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