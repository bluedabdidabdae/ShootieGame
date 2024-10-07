/*

    2d game, still working on its identity
    Copyright (C) 2024  blue_dabdidabdae

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "raylib.h"
#include "headers/global_types.h"
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