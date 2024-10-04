typedef enum StatesModel{
    MENU,
    PLAY,
    SETTINGS,
    EXITGAME,
    MEMERR
} States;

#define WIDTH 1335
#define HEIGT 675
#define WINDOWNAME "Shootie Shootie Game"
#define TARGETFPS 60

void MainMenuHandler(States *gameStatus);
int GameEngine();