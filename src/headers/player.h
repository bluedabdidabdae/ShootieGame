// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PLAYER_H
#define PLAYER_H

#define PLAYERSPEED 5.0f
#define PLAYERPROJECTILESPEED 17.0f

void PlayerShooting(uint frameCounter, ProjectileLL *projectileHead, Rectangle *player);
void UpdatePlayer(Rectangle *player);

#endif