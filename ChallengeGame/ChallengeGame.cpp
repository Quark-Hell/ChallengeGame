// ChallengeGame.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <ctime>

using namespace std;

#pragma region EngineClass
class Timer
{
public:
    void start()
    {
        m_StartTime = std::chrono::system_clock::now();
        m_bRunning = true;
    }

    void stop()
    {
        m_EndTime = std::chrono::system_clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds()
    {
        std::chrono::time_point<std::chrono::system_clock> endTime;

        if (m_bRunning)
        {
            endTime = std::chrono::system_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};
class GameObject {
public:
    COORD Pos;
public:
    string TextureObject;
    bool KillXY(GameObject &gameObject) {
        if (Pos.X < 0 || Pos.Y < 0) {
            return true;
        }
        else
        {
            return false;
        }
    }
};
COORD GetConsoleCursorPosition()
    {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hCon, &csbi))
        {
            return csbi.dwCursorPosition;
        }
        else
        {
            // The function failed. Call GetLastError() for details.
            COORD invalid = { 0, 0 };
            return invalid;
        }
    }
LPTSTR ReadConsoleOut(COORD coord) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    LPTSTR lpCharacter = new TCHAR[1];
    DWORD dwReaden = 0;
    ReadConsoleOutputCharacter(hCon, lpCharacter, 1, GetConsoleCursorPosition(), &dwReaden);
    return lpCharacter;
}
class Draw {
public:
    void EreseObject(COORD coord,GameObject object) {
        for (int i = 0;i < object.TextureObject.length();i++)
        {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            printf(" ");
            coord.X++;
        }
    }
public:
    void DrawObject(COORD coord,GameObject object) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        printf("%s", object.TextureObject.c_str());
    }
public:
    void DrawString(string Texture,COORD coord = GetConsoleCursorPosition()) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        printf("%s", Texture.c_str());
    }
};
Draw* draw;

vector<vector<string>> Map;
class Collision {
    bool СontinuedCollision() {
        //TODO:
        if (true) {
            return true;
        }
        else
        {
            return false;
        }
    }
public:
    bool StandartCollision(COORD coord) {
        LPTSTR lpCharacter = ReadConsoleOut(coord);
        string str;
        str = lpCharacter[0];
        if (str == " ") {
            return true;
        }
        else
        {
            return false;
        }
    }
};
Collision* collision;
/// <summary>
/// Класс перемещения объекта.Тут будет все виды перемещения
/// </summary>
class Controller {
public:
    /// <summary>
    /// Перемещение
    /// </summary>
    COORD ObjectMove(GameObject &MoveObject,int speed,int directionX,int directionY) {
        COORD collisionCoord = MoveObject.Pos;
#pragma region Костыль
        if (directionX < 0 || directionX == 0) {
            collisionCoord.X += speed * directionX;
            collisionCoord.Y += speed * directionY;
        }
        else
        {
            collisionCoord.X += speed * directionX + MoveObject.TextureObject.size() - 1;
            collisionCoord.Y += speed * directionY;
        }
#pragma endregion

        if (collision->StandartCollision(collisionCoord)) {
            draw->EreseObject(MoveObject.Pos, MoveObject);

            MoveObject.Pos.X += speed * directionX;
            MoveObject.Pos.Y += speed * directionY;

            draw->DrawObject(MoveObject.Pos, MoveObject);
      
        }
        return MoveObject.Pos;
    }
};
Controller* controller;
#pragma endregion

#pragma region EngineMethod
GameObject* CreateObject(string TextureObject, int StartXPos = 0, int StartYPos = 0) {
    GameObject* newObject = new GameObject;
    newObject->Pos.X = StartXPos;
    newObject->Pos.Y = StartYPos;
    newObject->TextureObject = TextureObject;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newObject->Pos);
    printf("%s", TextureObject.c_str());
    return newObject;
}

char GetUserInput() {
    char Input;
    Input = _getch();
    return Input;
}

void Start();
void Tick();
vector <GameObject> AllGameObjects;
void Render();

#pragma endregion

#pragma region Game

class GameSceneInfo {
public:
    string UPorDownBorder = "=================================================================\n";
public:
    string SideBorder = "||                                                             ||\n";
};
GameSceneInfo* gameSceneInfo;
GameObject* player;

Timer moveTimer;
Timer shootTimer;
Timer bulletMoveTimer;


void initializator() {
    gameSceneInfo = new GameSceneInfo();
    draw = new Draw();
    controller = new Controller();
}
void BuildGameScene() {
    COORD coord;

    draw->DrawString(gameSceneInfo->UPorDownBorder);
    for (int i = 0; i < 20; i++)
    {
        draw->DrawString(gameSceneInfo->SideBorder);
    }
    draw->DrawString(gameSceneInfo->UPorDownBorder);
}

#pragma region CustomMethods
bool Move(GameObject &player) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, FOREGROUND_GREEN);

    if (GetAsyncKeyState(0x41)) {
        player.Pos = controller->ObjectMove(player, 1, -1, 0);
        return true;
    }
    else if (GetAsyncKeyState(0x44)) {
        player.Pos = controller->ObjectMove(player, 1, 1, 0);
        return true;
    }
    else if (GetAsyncKeyState(0x57)) {
        player.Pos = controller->ObjectMove(player, 1, 0, -1);
        return true;
    }
    else if (GetAsyncKeyState(0x53)) {
        player.Pos = controller->ObjectMove(player, 1, 0, 1);
        return true;
    }
    else {
        return false;
    }
}

vector<GameObject> AllBullet;
bool Shoot(COORD pos) {
    if (GetAsyncKeyState(0x20)) {
        GameObject bullet;
        bullet.TextureObject = "---";
        bullet.Pos = pos;
        draw->DrawObject(bullet.Pos,bullet);
        AllBullet.push_back(bullet);
        return true;
    }
    else
    {
        return false;
    }
}

#pragma endregion


void Start() {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, FOREGROUND_GREEN);

    player = CreateObject("-||", 20, 7);

    /*
    LPTSTR lpCharacter = ReadConsoleOut();
    string str;
    str = lpCharacter[0];
    printf("%ls", lpCharacter);
    */

    moveTimer.start();
    shootTimer.start();
    bulletMoveTimer.start();
}
void Tick() {
    if (moveTimer.elapsedMilliseconds() > 50) {
        if (Move(*player)) {
            moveTimer.stop();
            moveTimer.start();
        }
    }
    if (shootTimer.elapsedMilliseconds() > 300) {
        COORD coord;
        coord.X = player->Pos.X - 3;
        coord.Y = player->Pos.Y;
        if (Shoot(coord)) {
            shootTimer.stop();
            shootTimer.start();
        }
    }
    if (bulletMoveTimer.elapsedMilliseconds() > 100) {
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hCon, FOREGROUND_RED);
        for (int i = 0; i < AllBullet.size(); i++)
        {
            AllBullet[i].Pos = controller->ObjectMove(AllBullet[i],1,-1,0);
        }
        bulletMoveTimer.stop();
        bulletMoveTimer.start();
    }
}
void Render() {

}

int main()
{
    initializator();
    BuildGameScene();

    //AllGameObjects.insert(CreateObject(3,3));
    Start();
    while (true)
    {
        Tick();
        Render();
    }
    COORD coord;
    coord.X = 0;
    coord.Y = 25;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

#pragma endregion
