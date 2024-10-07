// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef PLAYER_H
#define PLAYER_H

#define PLAYERSPEED 5.0f   

void UpdatePlayer(Rectangle *player);
void PlayerShooting(uint frameCounter, ProjectileLL *projectileHead, Rectangle *player);

#endif