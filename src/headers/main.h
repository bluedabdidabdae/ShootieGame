// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef MAIN_H
#define MAIN_H

//////////////////////////////////////////////////////////
typedef enum to_draw_status{
    MAINMENU,
    GAME,
    CLOSEGAME
}ToDraw;

typedef struct game_data_model{
    ToDraw toDraw;
    Vector2 mousePosition;
    Camera2D camera;
    Rectangle player;
    Rectangle mapBorder[4];
    EnemyLL *enemiesHead;
    ProjectileLL *projectileHead;
}GameDataS;
//////////////////////////////////////////////////////////

#endif