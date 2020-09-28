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
    string TextureObject = " ";
public:
    DWORD Color;
public:
    string Tag = "null";
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
};
WORD Attributes = 0;
class ConsoleInfo {
public:
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
public:
    LPTSTR ReadConsoleOut(COORD coord) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        LPTSTR lpCharacter = new TCHAR[1];
        DWORD dwReaden = 0;
        ReadConsoleOutputCharacter(hCon, lpCharacter, 1, GetConsoleCursorPosition(), &dwReaden);
        return lpCharacter;
    }
public:
    bool SetCursorPosition(COORD coord) {
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(hCon, coord);
        return true;
    }
};
ConsoleInfo* consoleInfo;

class Draw {
public:
    void SetConsoleColour(DWORD Colour,WORD* Attributes = &Attributes)
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
        consoleInfo->SetCursorPosition(coord);

        SetConsoleColour(object.Color);

        printf("%s", object.TextureObject.c_str());
    }
public:
    void DrawString(string Texture, COORD coord = consoleInfo->GetConsoleCursorPosition(), DWORD Color = 0) {
        SetConsoleColour(Color);
        consoleInfo->SetCursorPosition(coord);
        printf("%s", Texture.c_str());
    }
public:
    void EraseString(COORD StartCoord,COORD EndCoord) {

        for (int i = 0; i < EndCoord.X - StartCoord.X; i++)
        {
            COORD coord;
            coord.X = StartCoord.X + i;
            coord.Y = StartCoord.Y;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            printf(" ");
        }
    }
};
Draw* draw;

/// <summary>
/// 0-Collision
/// 1-Bullet
/// 2-Enemy
/// </summary>
vector<vector<GameObject*>> AllGameObjects;

class Collision {
public:
    GameObject* StandartCollision(COORD coord) {
        LPTSTR lpCharacter = consoleInfo->ReadConsoleOut(coord);
        string str;
        str = lpCharacter[0];
        if (str != " ") {
            for (int i = 0; i < AllGameObjects[0].size(); i++) {
                COORD start = AllGameObjects[0][i]->CollisionCoordInWorld[0];
                COORD end = AllGameObjects[0][i]->CollisionCoordInWorld[AllGameObjects[0][i]->CollisionCoordInWorld.size() - 1];
                COORD cursorPos = consoleInfo->GetConsoleCursorPosition();
                if (start.X <= cursorPos.X && end.X >= cursorPos.X) {
                    if (start.Y <= cursorPos.Y && end.Y >= cursorPos.Y)
                    {
                        return AllGameObjects[0][i];
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
#pragma region Оптимизированное решение
        if (directionX <= 0) {
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

            MoveObject.Pos.X += speed * directionX;
            MoveObject.Pos.Y += speed * directionY;

            MoveObject.CollisionCoordInWorld.clear();
            MoveObject.SetCollision();

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

GameObject* CreateObject(string TextureObject,bool Collision, int StartXPos = 0, int StartYPos = 0,DWORD Color = 15,string Tag = "null") {
    GameObject* newObject = new GameObject;

    newObject->Pos.X = StartXPos;
    newObject->Pos.Y = StartYPos;
    newObject->TextureObject = TextureObject;
    newObject->Tag = Tag;
    newObject->Color = Color;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newObject->Pos);
    
    draw->DrawObject(consoleInfo->GetConsoleCursorPosition(),*newObject);
    if (Collision) {
        AllGameObjects[0].push_back(newObject);
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

#pragma endregion

#pragma region Game

class GameSceneInfo {
};
GameSceneInfo* gameSceneInfo;
GameObject* player;

int PlayerLife = 3;
int PlayerScore = 0;

bool defeat = false;

Timer movePlayerTimer;
Timer shootTimer;
Timer bulletMoveTimer;
Timer spawnEnemyTimer;
Timer moveEnemyTimer;
Timer CollisionTimer;

void initializator() {
    gameSceneInfo = new GameSceneInfo();
    draw = new Draw();
    controller = new Controller();
    consoleInfo = new ConsoleInfo();
    AllGameObjects.resize(3);
}
void BuildGameScene() {
    draw->SetConsoleColour(15);

    GameObject* UpBorder = CreateObject("=================================================================",true, 0, 0);

    for (int i = 1; i < 20; i++)
    {
        GameObject* LeftBorder = CreateObject("||", true,0 ,i,15,"border");
        GameObject* Space = CreateObject("                                                             ", false, 2, i);
        GameObject* RightBorder = CreateObject("||", true, 2 + Space->TextureObject.size(), i,15, "border");
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

bool Shoot(COORD pos) {
    if (GetAsyncKeyState(0x20)) {
        GameObject* bullet = CreateObject("--",true,pos.X,pos.Y,4,"null");
        AllGameObjects[1].push_back(bullet);
        return true;
    }
    else
    {
        return false;
    }
}
bool SpawnEnemy() {
    COORD coord;
    coord.X = 3;
    coord.Y = rand() % 18 + 2;
    AllGameObjects[2].push_back(CreateObject("||-", true, coord.X, coord.Y, 6, "enemy"));
    return true;
}

void OutputScore() {
    draw->SetConsoleColour(14);

    COORD start;
    start.X = 13;
    start.Y = 22;

    int ScoreLength = to_string(PlayerScore).size();

    COORD end;
    end.X = start.X + ScoreLength;
    end.Y = 22;
    draw->EraseString(start, end);

    consoleInfo->SetCursorPosition(start);
    printf("%i",PlayerScore);
}
void OutputLife() {
    draw->SetConsoleColour(14);

    COORD start;
    start.X = 12;
    start.Y = 24;

    int ScoreLength = to_string(PlayerLife).size();

    COORD end;
    end.X = start.X + ScoreLength;
    end.Y = 24;
    draw->EraseString(start, end);

    consoleInfo->SetCursorPosition(start);
    printf("%i", PlayerLife);
}

void Effect() {
    COORD start;
    start.X = 2;
    start.Y = 1;

    COORD end;
    end.X = 63;
    end.Y = 20;

    for (int i = start.X; i < end.X;i++) {
        for (int x = start.Y; x < end.Y;x++) {

            COORD currentCoord;
            currentCoord.X = i;
            currentCoord.Y = x;

            LPTSTR currentLPTSTR = consoleInfo->ReadConsoleOut(currentCoord);

            consoleInfo->SetCursorPosition(currentCoord);
            if (currentLPTSTR[0] == ' ') {
                draw->DrawString(" ", consoleInfo->GetConsoleCursorPosition(), 75);
            }
            else
            {
                string str;
                str = currentLPTSTR[0];
                draw->DrawString(str, consoleInfo->GetConsoleCursorPosition(),75);
            }                 
        }
    }
    for (int i = start.X; i < end.X; i++) {
        for (int x = start.Y; x < end.Y; x++) {

            COORD currentCoord;
            currentCoord.X = i;
            currentCoord.Y = x;

            LPTSTR currentLPTSTR = consoleInfo->ReadConsoleOut(currentCoord);

            consoleInfo->SetCursorPosition(currentCoord);
            if (currentLPTSTR[0] == ' ') {
                draw->DrawString(" ", consoleInfo->GetConsoleCursorPosition(), 15);
            }
            else
            {
                string str;
                str = currentLPTSTR[0];
                draw->DrawString(str, consoleInfo->GetConsoleCursorPosition(), 15);
            }
        }
    }
}
void EffectWithErase() {
    COORD start;
    start.X = 2;
    start.Y = 1;

    COORD end;
    end.X = 63;
    end.Y = 20;

    for (int i = start.X; i < end.X; i++) {
        for (int x = start.Y; x < end.Y; x++) {

            COORD currentCoord;
            currentCoord.X = i;
            currentCoord.Y = x;

            draw->DrawString(" ", currentCoord, 75);
        }
    }
    for (int i = start.X; i < end.X; i++) {
        for (int x = start.Y; x < end.Y; x++) {

            COORD currentCoord;
            currentCoord.X = i;
            currentCoord.Y = x;

            draw->DrawString(" ", consoleInfo->GetConsoleCursorPosition(), 15);
        }
    }
}

void EnemyOverlap() {
    for (int i = 0; i < AllGameObjects[2].size(); i++) {
        if (AllGameObjects[2][i]->OverlappedObject != nullptr) {
            //удар с пулей
            if (AllGameObjects[2][i]->OverlappedObject->TextureObject == "--") {
                draw->EraseObject(*AllGameObjects[2][i]);

#pragma region Костыль
                for (int x = 0; x < AllGameObjects[0].size(); x++) {
                    if (AllGameObjects[2][i] == AllGameObjects[0][x]) {
                        AllGameObjects[0].erase(AllGameObjects[0].begin() + x);
                    }
                }
#pragma endregion

                AllGameObjects[2].erase(AllGameObjects[2].begin() + i);

                PlayerScore++;
                OutputScore();

                break;
            }
            if (AllGameObjects[2][i]->OverlappedObject->Tag == "player") {
                draw->EraseObject(*AllGameObjects[2][i]);

#pragma region Костыль
                for (int x = 0; x < AllGameObjects[0].size(); x++) {
                    if (AllGameObjects[2][i] == AllGameObjects[0][x]) {
                        AllGameObjects[0].erase(AllGameObjects[0].begin() + x);
                    }
                }
#pragma endregion

                AllGameObjects[2].erase(AllGameObjects[2].begin() + i);

                PlayerLife--;
                OutputLife();
                Effect();

                break;
            }
            //граница карты
            if (AllGameObjects[2][i]->OverlappedObject->Tag == "border")
            {
                draw->EraseObject(*AllGameObjects[2][i]);

#pragma region Костыль
                for (int x = 0; x < AllGameObjects[0].size(); x++) {
                    if (AllGameObjects[2][i] == AllGameObjects[0][x]) {
                        AllGameObjects[0].erase(AllGameObjects[0].begin() + x);
                    }
                }
#pragma endregion

                AllGameObjects[2].erase(AllGameObjects[2].begin() + i);

                PlayerLife--;
                Effect();

                break;
            }
        }
    }
}
void BulletOverlap() {
    for (int i = 0; i < AllGameObjects[1].size();i++) {
        if (AllGameObjects[1][i]->OverlappedObject != nullptr) {
            draw->EraseObject(*AllGameObjects[1][i]);

#pragma region Костыль
            for (int x = 0; x < AllGameObjects[0].size();x++) {
                if (AllGameObjects[1][i] == AllGameObjects[0][x]) {
                    AllGameObjects[0].erase(AllGameObjects[0].begin() + x);
                }
            }
#pragma endregion
            AllGameObjects[1].erase(AllGameObjects[1].begin() + i);

        }
    }
}

void Restart() {
    AllGameObjects[0].clear();
    AllGameObjects[1].clear();
    AllGameObjects[2].clear();

    PlayerLife = 3;
    PlayerScore = 0;
    defeat = false;

    EffectWithErase();
}

bool Defeat() {
    if (PlayerLife <= 0) {
        return true;
    }
    else
    {
        return false;
    }
}
void DefeatInfo() {
    COORD coord;
    coord.X = 0;
    coord.Y = 27;

    consoleInfo->SetCursorPosition(coord);
    draw->SetConsoleColour(5);

    printf("You defeat\n");
    printf("Press Space,if you want to try again\n");
    printf("Or press twice Esc for exit the game\n");

    while (true)
    {
        if (GetAsyncKeyState(0x20)) {
            Restart();
            break;
        }
        if (GetAsyncKeyState(0x1B)) {
            break;
        }
    }
}

#pragma endregion

void Start() {

    player = CreateObject("-||", true, 50, 7,2,"player");

    AllGameObjects[0].push_back(player);

    COORD coord;
    coord.X = 2;
    coord.Y = 22;
    consoleInfo->SetCursorPosition(coord);
    draw->SetConsoleColour(14);

    printf("Your Score: ");
    OutputScore();

    COORD output;
    output.X = 2;
    output.Y = 24;
    consoleInfo->SetCursorPosition(output);
    printf("Your Life:");
    OutputLife();
    
    movePlayerTimer.start();
    shootTimer.start();
    bulletMoveTimer.start();
    spawnEnemyTimer.start();
    moveEnemyTimer.start();
    CollisionTimer.start();
}
void Tick() {
    if (movePlayerTimer.elapsedMilliseconds() > 50) {
        if (Move(*player)) {
            movePlayerTimer.stop();
            movePlayerTimer.start();
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
        for (int i = 0; i < AllGameObjects[1].size(); i++)
        {
            AllGameObjects[1][i]->Pos = controller->ObjectMove(*AllGameObjects[1][i],1,-1,0);
        }
        bulletMoveTimer.stop();
        bulletMoveTimer.start();
    }
    if (spawnEnemyTimer.elapsedSeconds() > 4) {
        if (SpawnEnemy()) {
            spawnEnemyTimer.stop();
            spawnEnemyTimer.start();
        }
    }
    if (moveEnemyTimer.elapsedMilliseconds() >150) {
        for (int i = 0; i < AllGameObjects[2].size();i++) {
            controller->ObjectMove(*AllGameObjects[2][i],1,1,0);
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

int main()
{
    start:
    initializator();
    BuildGameScene();

    Start();
    while (defeat == false)
    {
        Tick();
        defeat = Defeat();
    }

    DefeatInfo();

    if (defeat == false) {
        goto start;
    }
}

#pragma endregion
