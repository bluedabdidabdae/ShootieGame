// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include "raylib.h"
#include "headers/global_types.h"

int CheckHitboxRec(Rectangle rect1, Rectangle rect2)
{
    return (rect1.x <= rect2.x + rect2.width &&
            rect1.y <= rect2.y + rect2.height &&
            rect1.x + rect1.height >= rect2.x &&
            rect1.y + rect1.width >= rect2.y);
}