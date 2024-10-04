#include "raylib.h"

#include "headers/main.h"

int main(int argc, char *argv[])
{
    States gameStatus;

    //SetExitKey(KEY_ESC);
    InitWindow(WIDTH, HEIGT, WINDOWNAME);
    SetTargetFPS(TARGETFPS);

    while(1)
    {
        MainMenuHandler(&gameStatus);
        switch(gameStatus)
        {
            case MENU: break;
            case PLAY:
                GameEngine();
                gameStatus = MENU;
            break;
            case EXITGAME:
                CloseWindow();
                return 0;
            break;
        }
    }
}