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

#define PLAYERPROJECTILESPEED 25.0f

void PlayerShooting(uint frameCounter, ProjectileLL *projectileHead, Rectangle *player, Vector2 *mousePosition)
{
    float Dx, Dy, tmp;
    float mouseX;
    float mouseY;
    ProjectileLL *aux;

    mouseX = mousePosition->x;
    mouseY = mousePosition->y;
    
    // aggiungo un proiettile in testa alla lista e lo inizializzo
    // con le coordinate ed il valore dei vettori per poi aggiornarne
    // la posizione in "UpdateProjectiles(...)"
    aux = projectileHead->next;
    projectileHead->next = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    projectileHead = projectileHead->next;
    projectileHead->next = aux;

    projectileHead->projectile = { player->x + player->width / 2,
                                   player->y + player->height / 2,
                                   10, 10 };
    projectileHead->color = YELLOW;

    Dx = projectileHead->projectile.x - mouseX;
    Dy = projectileHead->projectile.y - mouseY;

    tmp = abs(Dx) + abs(Dy);

    projectileHead->vX = PLAYERPROJECTILESPEED * (Dx / tmp);
    projectileHead->vY = PLAYERPROJECTILESPEED * (Dy / tmp);

    projectileHead->owner = PLAYER;
}

void UpdatePlayer(Rectangle *player)
{
    if (IsKeyDown(KEY_W)) player->y -= PLAYERSPEED;
    if (IsKeyDown(KEY_A)) player->x -= PLAYERSPEED;
    if (IsKeyDown(KEY_S)) player->y += PLAYERSPEED;
    if (IsKeyDown(KEY_D)) player->x += PLAYERSPEED;
    if (player->x < WALLTHICKNESS) player->x = WALLTHICKNESS;
    if (player->y < WALLTHICKNESS) player->y = WALLTHICKNESS;
    if (player->x > WIDTH-40) player->x = WIDTH-40;
    if (player->y > HEIGT-40) player->y = HEIGT-40;
}