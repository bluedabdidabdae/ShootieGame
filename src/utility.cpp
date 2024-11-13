// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include "raylib.h"
#include "headers/global_types.h"
#include "headers/utility.h"

int CheckHitboxMap(int level[MAPY][MAPX], Rectangle *entity)
{
    int i, ii;
    int x, y;

    x = entity->x/WALLTHICKNESS;
    y = entity->y/WALLTHICKNESS;

    for(i = y; i < y+3; i++)
    {
        for(ii = x; ii < x+3; ii++)
        {
            if(level[i][ii] > 1)
            {
                if(
                    entity->x <= WALLTHICKNESS*ii+WALLTHICKNESS &&
                    entity->y <= WALLTHICKNESS*i+WALLTHICKNESS &&
                    entity->x >= WALLTHICKNESS*ii-entity->width &&
                    entity->y >= WALLTHICKNESS*i-entity->height
                )
                    return 1;
            }
        }
    }
    return 0;
}

int CheckHitboxRec(Rectangle rect1, Rectangle rect2)
{
    return (rect1.x <= rect2.x + rect2.width &&
            rect1.y <= rect2.y + rect2.height &&
            rect1.x >= rect2.x - rect2.width &&
            rect1.y >= rect2.y - rect2.height);
}