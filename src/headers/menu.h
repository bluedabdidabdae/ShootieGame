// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef MENU_H
#define MAIN_H

typedef enum StatesModel{
    MENU,       // 0
    PLAY,       // 1
    SETTINGS,   // 2
    EXITGAME,   // 3
    MEMERR      // 4
} States;

void MainMenuHandler(States *gameStatus, Vector2 mousePosition);
States CheckCollisions(Vector2* mousePosition);

#endif