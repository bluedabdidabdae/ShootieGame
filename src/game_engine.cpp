// Copyright (C) 2024  blue_dabdidabdae
// full notice in main.cpp
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctime> // FOR LINUX
// #include <time.h> // FOR WINDOWS

#include "raylib.h"
#include "headers/game_engine.h"

int GameEngine()
{
    srand(time(NULL));

    uint frameCounter = 0;
    Rectangle player = { WIDTH/2-20, HEIGT/2-20, 40, 40 };

    EnemyLL *enemiesHead;
    if(SpawnEnemy(&enemiesHead, 40, 40) == -1)
        if(SpawnEnemy(&enemiesHead, 40, 40) == -1)
            if(SpawnEnemy(&enemiesHead, 40, 40) == -1)
                return -1;
    EnemyLL *lastEnemy = enemiesHead;

    ProjectileLL *projectileHead = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    projectileHead->next = NULL;

    SpawnEnemy(&lastEnemy->next, 120, 50);
    lastEnemy = lastEnemy->next;
    SpawnEnemy(&lastEnemy->next, 620, 50);
    lastEnemy = lastEnemy->next;
    SpawnEnemy(&lastEnemy->next, 894, 760);
    lastEnemy = lastEnemy->next;
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ WIDTH/2.0f, HEIGT/2.0f };
    camera.zoom = 0.6f;

    Rectangle mapBorder[] = { {0, 0, WIDTH+WALLTHICKNESS, WALLTHICKNESS}, 
                              {0, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS},
                              {0, HEIGT, WIDTH+WALLTHICKNESS, WALLTHICKNESS},
                              {WIDTH, 0, WALLTHICKNESS, HEIGT+WALLTHICKNESS} };

    while(1)
    {
        // Camera target follows player
        camera.target = (Vector2){ player.x + 20, player.y + 20 };

        frameCounter++;
        DrawGame(&camera, enemiesHead, &player, mapBorder, projectileHead);

        if(IsKeyPressed(KEY_M))
        {
            //if(GameMenuHandler() == 3)
            DeleteEnemies(enemiesHead);
            DeleteProjectiles(projectileHead);
            return 0;
        }
        else
        {
            UpdatePlayer(&player);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                PlayerShooting(frameCounter, projectileHead, &player);
            UpdateEnemies(enemiesHead, &player);
            SnapEnemies(enemiesHead, mapBorder);
            EnemiesShooting(enemiesHead, projectileHead, &player);
            UpdateProjectiles(projectileHead);
            CheckProjectilesBorders(projectileHead, mapBorder);
        }
    }
}

void PlayerShooting(uint frameCounter, ProjectileLL *projectileHead, Rectangle *player)
{
    float Dx, Dy, tmp;
    float mouseX;
    float mouseY;
    ProjectileLL *aux;

    mouseX = GetMouseX();
    mouseY = GetMouseY();
    
    // aggiungo un proiettile in testa alla lista e lo inizializzo
    // con le coordinate ed il valore dei vettori per poi aggiornarne
    // la posizione in "UpdateProjectiles(...)"
    aux = projectileHead->next;
    projectileHead->next = (ProjectileLL*)malloc(sizeof(ProjectileLL));
    projectileHead = projectileHead->next;
    projectileHead->next = aux;

    projectileHead->projectile = { player->x + player->width / 2,
                                   player->y + player->height / 2,
                                   10, 10 };
    projectileHead->color = YELLOW;

    Dx = projectileHead->projectile.x - mouseX;
    Dy = projectileHead->projectile.y - mouseY;

    tmp = abs(Dx) + abs(Dy);

    projectileHead->vX = PROJECTILESPEED * (Dx / tmp);
    projectileHead->vY = PROJECTILESPEED * (Dy / tmp);

    projectileHead->owner = PLAYER;
}

void ProjectilePop(ProjectileLL *prePop, ProjectileLL **toPop)
{
    prePop->next = (*toPop)->next;
    free(*toPop);
    *toPop = prePop;
    prePop = NULL;
}

void CheckProjectilesBorders(ProjectileLL *currentProjectile, Rectangle mapBorder[])
{
    ProjectileLL *previousProjectile;

    if(currentProjectile->next != NULL)
    {
        do{
            // passo al prossimo proiettile (a differenza della
            // linked list dei nemici questa mantiene il primo 
            // nodo vuoto ed allocato fino a fine partita,
            // in futuro le faccio uguali, dipende
            // con quale dei due modi mi trovo più comodo)
            previousProjectile = currentProjectile;
            currentProjectile = currentProjectile->next;

            // deleto i proiettili che impattano coi bordi della mappa
            // LASCIARE TUTTI GLI ELSE, ALTRIMENTI SI SFONDA LA MEMORIA
            // controlli per le collisioni tra il proiettile ed i 4 bordi mappa
            if(currentProjectile->projectile.x < mapBorder[1].x+WALLTHICKNESS)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.x > mapBorder[3].x-currentProjectile->projectile.width)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.y < mapBorder[0].y+WALLTHICKNESS)
                ProjectilePop(previousProjectile, &currentProjectile);
            else if(currentProjectile->projectile.y > mapBorder[2].y-currentProjectile->projectile.height)
                ProjectilePop(previousProjectile, &currentProjectile);

            // check se ho fatto il pop dell'ultimo elemento della lista
            if(currentProjectile == NULL)
                return;
            
            // check se ho finito la lista
        }while(currentProjectile->next != NULL);
    }
}

void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[])
{
    while(1){
        if(currentEnemy->enemy.x < mapBorder[1].x+WALLTHICKNESS)
            currentEnemy->enemy.x = mapBorder[1].x+WALLTHICKNESS;
        else if(currentEnemy->enemy.x > mapBorder[3].x-currentEnemy->enemy.width)
            currentEnemy->enemy.x = mapBorder[3].x-currentEnemy->enemy.width;
        
        if(currentEnemy->enemy.y < mapBorder[0].y+WALLTHICKNESS)
            currentEnemy->enemy.y = mapBorder[0].y+WALLTHICKNESS;
        else if(currentEnemy->enemy.y > mapBorder[2].y-currentEnemy->enemy.height)
            currentEnemy->enemy.y = mapBorder[2].y-currentEnemy->enemy.height;

        if(currentEnemy->next == NULL)
            break;
        currentEnemy = currentEnemy->next;
    }
}

void UpdateProjectiles(ProjectileLL *projectileHead)
{
    while(projectileHead->next != NULL)
    {
        projectileHead->projectile.x -= projectileHead->vX;
        projectileHead->projectile.y -= projectileHead->vY;
        projectileHead = projectileHead->next;
    }
}

void EnemiesShooting(EnemyLL *currentEnemy, ProjectileLL *projectileHead, Rectangle *player)
{
    float Dx, Dy, tmp;
    ProjectileLL *aux;

    while(currentEnemy != NULL)
    {
        // tiro a caso se il nemico spara o no
        if(rand()%1000 < 15)
        {
            // aggiungo un proiettile in coda alla lista e lo inizializzo
            // con le coordinate ed il valore dei vettori per poi aggiornarne
            // la posizione in "UpdateProjectiles(...)"
            aux = projectileHead->next;
            projectileHead->next = (ProjectileLL*)malloc(sizeof(ProjectileLL));
            projectileHead = projectileHead->next;
            projectileHead->next = aux;

            projectileHead->projectile = { currentEnemy->enemy.x+currentEnemy->enemy.width/2,
                                           currentEnemy->enemy.y+currentEnemy->enemy.height/2,
                                           10, 10 };
            projectileHead->color = PURPLE;

            Dx = projectileHead->projectile.x - (player->x + player->height / 2);
            Dy = projectileHead->projectile.y - (player->y + player->width / 2);

            tmp = abs(Dx) + abs(Dy);

            projectileHead->vX = PROJECTILESPEED * (Dx / tmp);
            projectileHead->vY = PROJECTILESPEED * (Dy / tmp);

            projectileHead->owner = ENEMY;
        }
        // passo al prossimo nemico
        currentEnemy = currentEnemy->next;
    }
}

void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player)
{
    float Dx, Dy, tmp;

    while(1){

        if(STILL == currentEnemy->behaviour)
        {
            if(rand()%1000 < 980)
                goto ignore_stuff;
            else
                currentEnemy->behaviour = BACKING;
        }
        else if(rand()%1000 < 10)
        {
            currentEnemy->behaviour = STILL;
            goto ignore_stuff;
        }
        
        Dx = currentEnemy->enemy.x - player->x;
        Dy = currentEnemy->enemy.y - player->y;
        
        tmp = abs(Dx) + abs(Dy);
        
        if(abs(Dx)+abs(Dy) > ENEMYMAXPDISTANCE)
            currentEnemy->behaviour = APPROACHING;
        else if(abs(Dx)+abs(Dy) < ENEMYMINPDISTANCE)
            currentEnemy->behaviour = BACKING;
        
        switch(currentEnemy->behaviour)
        {
            case BACKING:
                currentEnemy->enemy.x += ENEMYSPEED * (Dx / tmp);
                currentEnemy->enemy.y += ENEMYSPEED * (Dy / tmp);
            break;
            case APPROACHING:
                currentEnemy->enemy.x -= ENEMYSPEED * (Dx / tmp);
                currentEnemy->enemy.y -= ENEMYSPEED * (Dy / tmp);
            break;
        }
        
        currentEnemy->healthBar.x = currentEnemy->enemy.x;
        currentEnemy->healthBar.y = currentEnemy->enemy.y-20;
        

        ignore_stuff:

        if(currentEnemy->next == NULL)
            break;
        
        currentEnemy = currentEnemy->next;
    }
}

void UpdateTrackingEntity(EnemyLL *currentEnemy, Rectangle *player)
{
    float Dx, Dy, tmp;

    while(1){
        Dx = currentEnemy->enemy.x - player->x;
        Dy = currentEnemy->enemy.y - player->y;
        
        tmp = abs(Dx) + abs(Dy);

        currentEnemy->enemy.x += ENEMYSPEED * (Dx / tmp);
        currentEnemy->enemy.y += ENEMYSPEED * (Dy / tmp);

        if(currentEnemy->next == NULL)
            break;
        
        currentEnemy = currentEnemy->next;
    }
}

void DeleteProjectiles(ProjectileLL *head)
{
    ProjectileLL *tmp;
    while(head->next != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
    free(head);
}

void DeleteEnemies(EnemyLL *head)
{
    EnemyLL *tmp;
    while(1){
        tmp = head;
        head = head->next;
        free(tmp);
        if(tmp->next == NULL)
            break;
    }
}

void DrawGame(Camera2D *camera, EnemyLL *currentEnemy, Rectangle *player, Rectangle mapBorder[], ProjectileLL *projectileHead)
{
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(*camera);
            
            // drawing projectiles
            while(projectileHead->next != NULL)
            {
                DrawRectangleRec(projectileHead->projectile, projectileHead->color);
                projectileHead = projectileHead->next;
            }

            // drawing map borders
            for(int i = 0; i < 4; i++)
                DrawRectangleRec(mapBorder[i], BLUE);

            // drawing enemies from linked list of type *EnemyLL
            while(1){

                DrawRectangleRec(currentEnemy->healthBar, RED);
                DrawRectangle(currentEnemy->healthBar.x,
                              currentEnemy->healthBar.y,
                              currentEnemy->hitPoint,
                              currentEnemy->healthBar.height,
                              GREEN);
                DrawRectangleRec(currentEnemy->enemy, currentEnemy->color);
                if(currentEnemy->next == NULL)
                    break;
                currentEnemy = currentEnemy->next;
            }
            
            // drawing player
            DrawRectangleRec(*player, RAYWHITE);
        EndMode2D();
    EndDrawing();
}

void UpdatePlayer(Rectangle *player)
{
    if (IsKeyDown(KEY_W)) player->y -= PLAYERSPEED;
    if (IsKeyDown(KEY_A)) player->x -= PLAYERSPEED;
    if (IsKeyDown(KEY_S)) player->y += PLAYERSPEED;
    if (IsKeyDown(KEY_D)) player->x += PLAYERSPEED;
    if (player->x < WALLTHICKNESS) player->x = WALLTHICKNESS;
    if (player->y < WALLTHICKNESS) player->y = WALLTHICKNESS;
    if (player->x > WIDTH-40) player->x = WIDTH-40;
    if (player->y > HEIGT-40) player->y = HEIGT-40;
}

int SpawnEnemy(EnemyLL **destination, float x, float y)
{
    *destination = (EnemyLL*)malloc(sizeof(EnemyLL));
    
    if(*destination != NULL)
    {
        (*destination)->enemy = { x, y, 40, 40 };
        (*destination)->color = BROWN;
        (*destination)->next = NULL;
        (*destination)->behaviour = BACKING;
        (*destination)->hitPoint = 25;
        (*destination)->healthBar = { x, y-20, 40, 10 };
        return 0;
    }
    return -1;
}