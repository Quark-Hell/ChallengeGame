// ChallengeGame.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
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
    vector<COORD> CollisionCoordInWorld;
public:
    GameObject* OverlappedObject;
public:
    COORD Pos;
public:
    string TextureObject;
public:
    void SetCollision() {
        //SetCollisionCOORD
        COORD coord;
        for (int i = 0; i < TextureObject.size();i++) {
            if (TextureObject[i] != ' ') {
                coord.X = Pos.X + i;
                coord.Y = Pos.Y;
                CollisionCoordInWorld.push_back(coord);
            }
        }
    }
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
    void SetConsoleColour(WORD* Attributes, DWORD Colour)
    {
        CONSOLE_SCREEN_BUFFER_INFO Info;
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hStdout, &Info);
        *Attributes = Info.wAttributes;
        SetConsoleTextAttribute(hStdout, Colour);
    }
public:
    void ResetConsoleColour(WORD Attributes)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
    }
public:
    void EraseObject(GameObject object) {
        for (int i = 0;i < object.TextureObject.length();i++)
        {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), object.Pos);
            printf(" ");
            object.Pos.X++;
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

vector<GameObject*> AllGameObjectWithCollision;
class Collision {
public:
    GameObject* StandartCollision(COORD coord) {
        LPTSTR lpCharacter = ReadConsoleOut(coord);
        string str;
        str = lpCharacter[0];
        if (str != " ") {
            for (int i = 0; i < AllGameObjectWithCollision.size();i++) {
                for (int x = 0; x < AllGameObjectWithCollision[i]->CollisionCoordInWorld.size();x++) {
                    if (AllGameObjectWithCollision[i]->CollisionCoordInWorld[x].X == GetConsoleCursorPosition().X) {
                        if (AllGameObjectWithCollision[i]->CollisionCoordInWorld[x].Y == GetConsoleCursorPosition().Y)
                        {
                            return AllGameObjectWithCollision[i];
                        }
                    }
                }
            }
        }
        return nullptr;
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
        MoveObject.OverlappedObject = collision->StandartCollision(collisionCoord);
        if (MoveObject.OverlappedObject == nullptr) {

            draw->EraseObject(MoveObject);

            MoveObject.CollisionCoordInWorld.clear();
            MoveObject.SetCollision();

            MoveObject.Pos.X += speed * directionX;
            MoveObject.Pos.Y += speed * directionY;

            draw->DrawObject(MoveObject.Pos, MoveObject);
      
        }
        else
        {
            MoveObject.OverlappedObject->OverlappedObject = &MoveObject;
        }
        return MoveObject.Pos;
    }
};
Controller* controller;
#pragma endregion

#pragma region EngineMethod

GameObject* CreateObject(string TextureObject,bool Collision, int StartXPos = 0, int StartYPos = 0) {
    GameObject* newObject = new GameObject;

    newObject->Pos.X = StartXPos;
    newObject->Pos.Y = StartYPos;
    newObject->TextureObject = TextureObject;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newObject->Pos);
    printf("%s", TextureObject.c_str());
    if (Collision) {
        AllGameObjectWithCollision.push_back(newObject);
        newObject->SetCollision();
    }
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
};
GameSceneInfo* gameSceneInfo;
GameObject* player;

Timer movePlayerTimer;
Timer shootTimer;
Timer bulletMoveTimer;
Timer spawnEnemyTimer;
Timer moveEnemyTimer;
Timer CollisionTimer;

WORD Attributes = 0;


void initializator() {
    gameSceneInfo = new GameSceneInfo();
    draw = new Draw();
    controller = new Controller();
}
void BuildGameScene() {
    //draw->ResetConsoleColour(Attributes);

    GameObject* UpBorder = CreateObject("=================================================================",true, 0, 0);

    for (int i = 1; i < 20; i++)
    {
        GameObject* SideBorder = CreateObject("||                                                             ||", true,0 ,i );
    }

    GameObject* DownBorder = CreateObject("=================================================================", true, 0, 20);
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

vector<GameObject*> AllBullet;
bool Shoot(COORD pos) {
    if (GetAsyncKeyState(0x20)) {
        GameObject* bullet = CreateObject("--",true,pos.X,pos.Y);
        AllBullet.push_back(bullet);
        return true;
    }
    else
    {
        return false;
    }
}
vector<GameObject*> AllEnemy;
bool SpawnEnemy() {
    COORD coord;
    coord.X = 2;
    coord.Y = rand() % 20 + 2;
    AllEnemy.push_back(CreateObject("||-",true, coord.X, coord.Y));
    return true;
}

void EnemyOverlap() {
    for (int i = 0; i < AllEnemy.size();i++) {
        if (AllEnemy[i]->OverlappedObject != nullptr && AllEnemy[i]->OverlappedObject->TextureObject == "--") {
            draw->EraseObject(*AllEnemy[i]);
            AllEnemy.erase(AllEnemy.begin() + i);
        }
    }
}
void BulletOverlap() {
    for (int i = 0; i < AllBullet.size();i++) {
        if (AllBullet[i]->OverlappedObject != nullptr) {
            draw->EraseObject(*AllBullet[i]);
            AllBullet.erase(AllBullet.begin() + i);
        }
    }
}

#pragma endregion


void Start() {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, FOREGROUND_GREEN);

    player = CreateObject("-||", true, 50, 7);
    AllGameObjectWithCollision.push_back(player);

    /*
    LPTSTR lpCharacter = ReadConsoleOut();
    string str;
    str = lpCharacter[0];
    printf("%ls", lpCharacter);
    */

    movePlayerTimer.start();
    shootTimer.start();
    bulletMoveTimer.start();
    spawnEnemyTimer.start();
    moveEnemyTimer.start();
    CollisionTimer.start();
}
void Tick() {
    if (movePlayerTimer.elapsedMilliseconds() > 50) {
        draw->SetConsoleColour(&Attributes,FOREGROUND_GREEN);
        if (Move(*player)) {
            movePlayerTimer.stop();
            movePlayerTimer.start();
        }
    }
    if (shootTimer.elapsedMilliseconds() > 700) {
        COORD coord;
        coord.X = player->Pos.X - 3;
        coord.Y = player->Pos.Y;
        if (Shoot(coord)) {
            shootTimer.stop();
            shootTimer.start();
        }
    }
    if (bulletMoveTimer.elapsedMilliseconds() > 100) {
        draw->SetConsoleColour(&Attributes,FOREGROUND_RED);
        for (int i = 0; i < AllBullet.size(); i++)
        {
            AllBullet[i]->Pos = controller->ObjectMove(*AllBullet[i],1,-1,0);
        }
        bulletMoveTimer.stop();
        bulletMoveTimer.start();
    }
    if (spawnEnemyTimer.elapsedSeconds() > 4) {
        draw->SetConsoleColour(&Attributes, FOREGROUND_RED | FOREGROUND_GREEN);
        if (SpawnEnemy()) {
            spawnEnemyTimer.stop();
            spawnEnemyTimer.start();
        }
    }
    if (moveEnemyTimer.elapsedMilliseconds() > 200) {
        draw->SetConsoleColour(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        for (int i = 0; i < AllEnemy.size();i++) {
            controller->ObjectMove(*AllEnemy[i],1,1,0);
        }
        moveEnemyTimer.stop();
        moveEnemyTimer.start();
    }
    if (CollisionTimer.elapsedMilliseconds() > 50) {
        EnemyOverlap();
        BulletOverlap();
        CollisionTimer.stop();
        CollisionTimer.start();
    }
}
void Render() {

}

int main()
{
    initializator();
    BuildGameScene();

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
