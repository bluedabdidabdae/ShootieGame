#define ENEMYSPEED 3.0f
#define ENEMYMAXPDISTANCE 500
#define ENEMYMINPDISTANCE 400
#define WIDTH 1335
#define HEIGT 675
#define WINDOWNAME "Shootie Shootie Game"
#define TARGETFPS 60
#define MAINMENUBUTTONWIDTH 760
#define MAINMENUBUTTONHEIGT 50
#define FADEVALUE 0.2
#define MAINMENUBUTTONX 300
#define MAINMENUBUTTONY 270
#define MAINMENUTEXTCOLOR RAYWHITE
#define PLAYERSPEED 5.0f
#define PROJECTILESPEED 12.0f
#define PLAYERPROJECTILESPEED 17.0f
//////////////////////////////////////////////////////////
typedef enum behaviour_model{
        APPROACHING,
        BACKING,
        STILL
}Behaviour;
//////////////////////////////////////////////////////////
typedef struct Enemy_Linked_List{
    Rectangle enemy;
    Color color;
    Behaviour behaviour;
    Rectangle healthBar;
    int hitPoint;
    Enemy_Linked_List *next;
}EnemyLL;
//////////////////////////////////////////////////////////
typedef unsigned int uint;
//////////////////////////////////////////////////////////
typedef enum StatesModel{
    MENU,       // 0
    PLAY,       // 1
    SETTINGS,   // 2
    EXITGAME,   // 3
    MEMERR      // 4
} States;
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
typedef enum projectile_owner_model{
    ENEMY,
    PLAYER
}ProjectileOwner;
//////////////////////////////////////////////////////////
typedef struct Projectile_linked_List{
    Rectangle projectile;
    Color color;
    float vX;
    float vY;
    ProjectileOwner owner;
    Projectile_linked_List *next;
}ProjectileLL;
//////////////////////////////////////////////////////////
int SpawnEnemy(EnemyLL *destination, float x, float y);
void CompletelyDeleteAllEnemies(EnemyLL *head);
void UpdateEnemies(EnemyLL *currentEnemy, Rectangle *player);
void EnemiesShooting(EnemyLL *currentEnemy, ProjectileLL *projectileHead, Rectangle *player);
void SnapEnemies(EnemyLL *currentEnemy, Rectangle mapBorder[]);
int GameEngine(DrawArgsS *drawInfo);
void* DrawGame(void* dataPointer);
void *HandleGraphics(void* data);
void DrawMenu();
void MainMenuHandler(States *gameStatus, Vector2 mousePosition);
int GameEngine();
States CheckCollisions(Vector2* mousePosition);
void UpdatePlayer(Rectangle *player);
void PlayerShooting(uint frameCounter, ProjectileLL *projectileHead, Rectangle *player);