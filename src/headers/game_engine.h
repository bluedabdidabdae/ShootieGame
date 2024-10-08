// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

typedef struct draw_args_model{
    Camera2D* camera;
    EnemyLL* currentEnemy;
    Rectangle* player;
    Rectangle* mapBorder;
    ProjectileLL* projectileHead;
}DrawArgsS;

int GameEngine();
void* DrawGame(void* dataPointer);
//void DrawGame(Camera2D *camera, EnemyLL *currentEnemy, Rectangle *player, Rectangle mapBorder[], ProjectileLL *projectileHead);

#endif