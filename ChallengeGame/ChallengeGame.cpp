// ChallengeGame.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <ctime>

using namespace std;

#pragma region Заголов.h
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
#pragma region EngineTyps
class GameObject {
public:
    COORD Pos;
public:
    string TextureObject;
};
class Draw {
public:
    void EreseObject(COORD coord,GameObject object) {
        for (int i = 0;i < object.TextureObject.length() - 1;i++)
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
};
Draw* draw;
class Collision {

};

/// <summary>
/// Класс перемещения объекта.Тут будет все виды перемещения
/// </summary>
class Controller {
public:
    /// <summary>
    /// Перемещение для персонажа.Кардинальных отличий не имеет,но более удобно
    /// </summary>
    void PlayerMove(GameObject &MoveObject,int speed,int directionX,int directionY) {

        draw->EreseObject(MoveObject.Pos, MoveObject);

        MoveObject.Pos.X += speed * directionX;
        MoveObject.Pos.Y += speed * directionY;
        draw->DrawObject(MoveObject.Pos, MoveObject);
    }
    /// <summary>
    /// Перемещение для других объектов
    /// </summary>
    void ObjectMove();
};
Controller* controller;
/// <summary>
/// Стартовая постройка уровня.Срабатывает только в старте
/// </summary>
class StartBuildLevel {
public:
    void BuildGameScene(string GameLine) {
        printf("%s",GameLine.c_str());
    }
};
StartBuildLevel* startBuildLevel;
/// <summary>
/// Постройка уровня для нового кадра
/// </summary>
class ReBuildLevel {
public:
    void ChangeGameScene(GameObject gameObject);
};
ReBuildLevel* reBuildLevel;
#pragma endregion
#pragma region GameMethod
void Start();
void Tick();
void Render();
char GetUserInput() {
    char Input;
    Input = _getch();
    return Input ;
}
GameObject CreateObject(string TextureObject,int StartXPos = 0,int StartYPos = 0) {
    GameObject newObject;
    newObject.Pos.X = StartXPos;
    newObject.Pos.Y = StartYPos;
    newObject.TextureObject = TextureObject;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newObject.Pos);
    printf("%s", TextureObject.c_str());
    return newObject;
}
#pragma endregion
#pragma region Game
#pragma region GameField
class GameSceneInfo {
public:
    string UPorDownBorder = "=================================================================\n";
public:
    string SideBorder = "||                                                             ||\n";
};
#pragma endregion
list <GameObject> AllGameObjects;
#pragma endregion
#pragma endregion

GameSceneInfo* gameSceneInfo;


void initializator() {
    startBuildLevel = new StartBuildLevel();
    reBuildLevel = new ReBuildLevel();
    gameSceneInfo = new GameSceneInfo();
    draw = new Draw();
    controller = new Controller();
}
bool Move(GameObject &player) {

    if (GetAsyncKeyState(0x41)) {
        controller->PlayerMove(player, 1, -1, 0);
        return true;
    }
    else if (GetAsyncKeyState(0x44)) {
        controller->PlayerMove(player, 1, 1, 0);
        return true;
    }
    else if (GetAsyncKeyState(0x57)) {
        controller->PlayerMove(player, 1, 0, -1);
        return true;
    }
    else if (GetAsyncKeyState(0x53)) {
        controller->PlayerMove(player, 1, 0, 1);
        return true;
    }
    else {
        return false;
    }
}

list<GameObject> AllBullet;
bool Shoot(COORD pos) {
    if (GetAsyncKeyState(0x20)) {
        GameObject bullet;
        bullet.Pos = pos;
        bullet.TextureObject = '--';
        AllBullet.push_back(bullet);
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    initializator();

#pragma region BuildScene
    startBuildLevel->BuildGameScene(gameSceneInfo->UPorDownBorder);
    for (int i = 0; i < 20; i++)
    {
        startBuildLevel->BuildGameScene(gameSceneInfo->SideBorder);
    }
    startBuildLevel->BuildGameScene(gameSceneInfo->UPorDownBorder);
#pragma endregion

    //AllGameObjects.insert(CreateObject(3,3));
    GameObject player = CreateObject("-||\n", 20, 7);

    Timer moveTimer;
    Timer shootTimer;
    moveTimer.start();
    shootTimer.start();
    while (true)
    {
        while (shootTimer.elapsedMilliseconds() > 300) {
            if (Shoot(player.Pos)) {
                shootTimer.start();
            }
        }
        while (moveTimer.elapsedMilliseconds() > 50) {
            if (Move(player)) {
                moveTimer.start();
            }
        }
    }
       
    COORD coord;
    coord.X = 0;
    coord.Y = 25;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

