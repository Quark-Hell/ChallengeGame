// ChallengeGame.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <list>
#include <windows.h>

using namespace std;

#pragma region Заголов.h
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
};
class Collision {

};
Draw* draw;
/// <summary>
/// Класс перемещения объекта.Тут будет все виды перемещения
/// </summary>
class Controller {
    /// <summary>
    /// Перемещение для персонажа.Кардинальных отличий не имеет,но более удобно
    /// </summary>
    void PlayerMove(GameObject MoveObject,int speed,int direction) {
        COORD EreaseCoord;
        EreaseCoord.X = MoveObject.Pos.X;
        EreaseCoord.Y = MoveObject.Pos.Y;
        draw->EreseObject(EreaseCoord, MoveObject);

        COORD DrawCoord;
        DrawCoord.X = MoveObject.Pos.X + direction + speed;
        DrawCoord.Y = MoveObject.Pos.Y + direction + speed;
        draw->DrawObject(DrawCoord, MoveObject);
    }
    /// <summary>
    /// Перемещение для других объектов
    /// </summary>
    void ObjectMove();
};
/// <summary>
/// Стартовая постройка уровня.Срабатывает только в старте
/// </summary>
class StartBuildLevel {
public:
    void BuildGameScene(string GameLine) {
        printf("%s",GameLine.c_str());
    }
};
/// <summary>
/// Постройка уровня для нового кадра
/// </summary>
class ReBuildLevel {
public:
    void ChangeGameScene(GameObject gameObject);
};
#pragma endregion
#pragma region GameMethod
void Start();
void Tick();
void Render();
void GetUserInput() {

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

StartBuildLevel* startBuildLevel;
ReBuildLevel* reBuildLevel;
GameSceneInfo* gameSceneInfo;


void initializator() {
    startBuildLevel = new StartBuildLevel();
    reBuildLevel = new ReBuildLevel();
    gameSceneInfo = new GameSceneInfo();
    draw = new Draw();
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
    GameObject player = CreateObject("-||\n",3,3);


    COORD coord;
    coord.X = 0;
    coord.Y = 25;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

