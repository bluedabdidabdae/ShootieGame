// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <vector>

#include "raylib.h"
#include "headers/global_types.h"
#include "headers/utility.h"

int CheckHitboxMap(Rectangle entity, MapS &map, std::vector<CustomTexture2D> &mapTextures)
{
    int i, ii;
    int x, y;
    int ret = 0;

    x = entity.x/WALLTHICKNESS;
    y = entity.y/WALLTHICKNESS;

    if(entity.x/WALLTHICKNESS < 0 || entity.y/WALLTHICKNESS < 0)
        ret = 1;

    for(i = y; i < y+2 && i < map.sizeY && !ret; i++)
    {
        for(ii = x; ii < x+2 && ii < map.sizeX && !ret; ii++)
        {
            if(mapTextures.size() <= map.bitmap[i][ii] || 0 > map.bitmap[i][ii])
                ret = 1;
            
            // HITBOX_START its halfway in the enum type that
            // defines various types of blocks, it splits it
            // in two halves (hitbox and no hitbox)
            else if(HITBOX_START < mapTextures[map.bitmap[i][ii]].blockType)
            {
                if(
                    entity.x < WALLTHICKNESS*ii+WALLTHICKNESS &&
                    entity.y < WALLTHICKNESS*i+WALLTHICKNESS &&
                    entity.x > WALLTHICKNESS*ii-entity.width &&
                    entity.y > WALLTHICKNESS*i-entity.height
                )
                {
                    ret = 1;
                }
            }
        }
    }
    return ret;
}

int CheckHitboxRec(Rectangle rect1, Rectangle rect2)
{
    return (rect1.x < rect2.x + rect2.width &&
            rect1.y < rect2.y + rect2.height &&
            rect1.x > rect2.x - rect2.width &&
            rect1.y > rect2.y - rect2.height);
}