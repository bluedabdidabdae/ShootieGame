// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef MENU_H
#define MAIN_H
#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define FADEVALUE 0.2
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270
#define MAINMENUTEXTCOLOR RAYWHITE

void MainMenuHandler(States *gameStatus, Vector2 mousePosition);
States CheckCollisions(Vector2* mousePosition);
#endif