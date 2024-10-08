// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#ifndef ENEMIES_H
#define ENEMIES_H
//////////////////////////////////////////////////////////
typedef enum behaviour_model{
        APPROACHING,
        BACKING,
        STILL
}Behaviour;
typedef struct Enemy_Linked_List{
    Rectangle enemy;
    Color color;
    Behaviour behaviour;
    Rectangle healthBar;
    int hitPoint;
    Enemy_Linked_List *next;
}EnemyLL;
//////////////////////////////////////////////////////////

int SpawnEnemy(EnemyLL *destination, float x, float y);
void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[]);
void EnemiesShooting(EnemyLL *currentEnemy, ProjectileLL *projectileHead, Rectangle *player);
void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player);
void CompletelyDeleteAllEnemies(EnemyLL *head);

#endif