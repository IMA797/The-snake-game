/******************************************************************
*                 Л е т н я я   п р а к т и к а                   *
*-----------------------------------------------------------------*
* Project Type  : Console Application                             *
* Project Name  : Praktika_zadanie_3                              *
* File Name     : main.cpp                                        *
* Language      : C++                                             *
* Programmer    : Исаев Магомед Абдурахманович                    *
* Created       : 29/03/26                                        *
* Last Revision : 12/04/26                                        *
* Comment       : Игра "Змейка"                                   *
******************************************************************/

#include <iostream>
#include <windows.h>
#include <ctime>
#include <vector>
#include "snake.h"

using namespace std;

/*hdc - дескриптор контекста устройства
buddefDC - буфер (он скрытый)
bufferBitmap - картинка, куда сначала все отрисовывается
whiteBrush - кисть для заливки фона
borderBrush - кисть для заливки границ (пустая)
borderPen - перо для границ
hwnd - дескриптор окна, чтоб знать где рисовать

Сразу моя картинка не рисуется на экран, в начале она отрисовывается в памяти а потом копируется на экран*/
HDC hdc, bufferDC;
HBITMAP bufferBitmap;
HBRUSH whiteBrush, borderBrush;
HPEN borderPen;
HWND hwnd;

//Размеры экрана и игрового поля
int screenWidth, screenHeight;
int fieldX, fieldY, fieldW, fieldH;

//Функция расстояния для проверок
int Dist2(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

/*
Инициализация буфера

Если рисовать напрямую на экран, то он будет мерцать
Чтобы избежать подрыгиваний, вначале все рисуется в памяти bufferDC
А потом просто копируем на экран
*/
void InitBuffer()
{
    //Создаем виртуальный экран в памяти
    bufferDC = CreateCompatibleDC(hdc);
    //Создаем пустую картинку рахмером с окно
    bufferBitmap = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
    //Привязываем картинку к виртуальному экрану
    SelectObject(bufferDC, bufferBitmap);
}

//отрисовка игрового поля
void DrawField()
{
    //Берем перо для отрисовки границы и borderBrush который не закрашивает внутри экрана ничего
    SelectObject(bufferDC, borderPen);
    SelectObject(bufferDC, borderBrush);
    //Рисуем черный контур и внутри пусто
    Rectangle(bufferDC, fieldX, fieldY, fieldX + fieldW, fieldY + fieldH);
}

//Отрисовка игры
void DrawAll(SnakeHead* head, vector<SnakeBody*>& body, Circle* foodCircle, Triangle* foodTriangle, vector<Enemy*>& enemies, vector<Wall*>& walls)
{
    //Очистка экрана, то есть заливка его большим прямоугольником 
    SelectObject(bufferDC, whiteBrush);
    Rectangle(bufferDC, -500, -500, screenWidth + 500, screenHeight + 500);

    //Рисуем рамку экрана
    DrawField();

    //Тело змейки (рисуем с конца, чтобы голова была сверху)
    for (int i = (int)body.size() - 1; i >= 0; i--)
        body[i]->DrawToDC(bufferDC);

    //Рисуем голову
    head->DrawToDC(bufferDC);

    //Рисуем еду 
    foodCircle->DrawToDC(bufferDC);
    foodTriangle->DrawToDC(bufferDC);

    // Рисуем врагов
    for (int i = 0; i < (int)enemies.size(); i++)
        enemies[i]->DrawToDC(bufferDC);

    // Рисуем стены
    for (int i = 0; i < (int)walls.size(); i++)
        walls[i]->DrawToDC(bufferDC);

    //Перенос на экран
    BitBlt(hdc, 0, 0, screenWidth, screenHeight, bufferDC, 0, 0, SRCCOPY);
}

//Проверка выхода за игровое поле
bool CheckBoundary(int x, int y, int radius)
{
    /*Проверка, что:
    левый край объекта не левее левой границы поля
    правй край объекта не правее правой границы поля
    верхний край объейкта не выше верхней границы поля 
    нижний край объекта не ниже нижней границы поля*/
    return (x - radius >= fieldX && x + radius <= fieldX + fieldW &&
        y - radius >= fieldY && y + radius <= fieldY + fieldH);
}

//Перекраска тела
void recolorBody(vector<SnakeBody*>& body, COLORREF newColor)
{
    for (int i = 0; i < (int)body.size(); i++)
    {
        body[i]->SetFillColor(newColor);
        body[i]->SetBorderColor(newColor);
    }
}

//Проверка, свободна ли позиция для еды, чтоб она не была за пределами игрового поля и не слишком близко к врагам
bool IsPositionFreeForFood(int x, int y, vector<Enemy*>& enemies, vector<Wall*>& walls)
{
    //Проверка границ поля с отступом для еды
    //Если хоть одно условие истинно - то еда вылезает за границу
    if (x - 15 < fieldX || x + 15 > fieldX + fieldW ||
        y - 15 < fieldY || y + 15 > fieldY + fieldH)
        return false;

    // Проверка, что еда не слищком близка к врагу
    for (int i = 0; i < (int)enemies.size(); i++)
    {
        //Разница координат между едой и врагом
        int dx = x - enemies[i]->GetX();
        int dy = y - enemies[i]->GetY();
        //Квадрат расстояний
        int dist2 = dx * dx + dy * dy;
        if (dist2 < 1600) // 40*40 - минимальное расстояние, чтоб еда и враги не были слишком близки
            return false;
    }

    // Проверка, что еда не на стене
    for (int i = 0; i < (int)walls.size(); i++)
    {
        // Проверка пересечения круга еды (радиус 12) с прямоугольником стены
        if (x + 12 > walls[i]->GetX() && x - 12 < walls[i]->GetX() + walls[i]->GetW() &&
            y + 12 > walls[i]->GetY() && y - 12 < walls[i]->GetY() + walls[i]->GetH())
        {
            return false;
        }
    }
    return true;
}

// Проверка столкновения со стенами
bool CheckWallCollision(int x, int y, int radius, vector<Wall*>& walls)
{

    for (int i = 0; i < (int)walls.size(); i++)
    {
        // Проверка пересечения квадрата головы с прямоугольником стены
        if (x + radius > walls[i]->GetX() && //Правый край головы > левый край стены
            x - radius < walls[i]->GetX() + walls[i]->GetW() && //Левый край головы < правый край стены
            y + radius > walls[i]->GetY() && //Нижний край головы > верхний край стены
            y - radius < walls[i]->GetY() + walls[i]->GetH()) //Верхний край головы < нижний край стены
        {
            return true;
        }
    }
    return false;
}

//Проверка, свободна ли позиция от змеи, еды, врагов и стен
bool IsPositionFreeForSpawn(int x, int y, int radius,
    SnakeHead* head, vector<SnakeBody*>& body,
    Circle* foodCircle, Triangle* foodTriangle,
    vector<Enemy*>& enemies, vector<Wall*>& walls)
{
    //Проверка границ поля
    if (!CheckBoundary(x, y, radius))
        return false;

    //Проверка головы
    int dx = x - head->GetX();
    int dy = y - head->GetY();
    if (dx * dx + dy * dy < (radius + head->GetRadius()) * (radius + head->GetRadius()))
        return false;

    //Проверка тела
    for (int i = 0; i < (int)body.size(); i++)
    {
        dx = x - body[i]->GetX();
        dy = y - body[i]->GetY();
        if (dx * dx + dy * dy < (radius + body[i]->GetRadius()) * (radius + body[i]->GetRadius()))
            return false;
    }

    //Проверка еды (круг)
    dx = x - foodCircle->GetX();
    dy = y - foodCircle->GetY();
    if (dx * dx + dy * dy < (radius + foodCircle->GetRadius()) * (radius + foodCircle->GetRadius()))
        return false;

    //Проверка еды (треугольник)
    dx = x - foodTriangle->GetX();
    dy = y - foodTriangle->GetY();
    if (dx * dx + dy * dy < (radius + foodTriangle->GetRadius()) * (radius + foodTriangle->GetRadius()))
        return false;

    //Проверка существующих врагов
    for (int i = 0; i < (int)enemies.size(); i++)
    {
        dx = x - enemies[i]->GetX();
        dy = y - enemies[i]->GetY();
        if (dx * dx + dy * dy < (radius + enemies[i]->GetRadius()) * (radius + enemies[i]->GetRadius()))
            return false;
    }

    //Проверка стен
    if (CheckWallCollision(x, y, radius, walls))
        return false;

    return true;
}

//Проверка, не пересекаются ли две стены
bool WallsCollide(Wall* w1, Wall* w2)
{
    /*Два прямоугольника не пересекаются, если один находится полностью:
    слева от другого
    справа от другого
    выше другого
    ниже другого*/
    return !(w1->GetX() + w1->GetW() <= w2->GetX() ||
        w2->GetX() + w2->GetW() <= w1->GetX() ||
        w1->GetY() + w1->GetH() <= w2->GetY() ||
        w2->GetY() + w2->GetH() <= w1->GetY());
}

//Проверка, что новая стена не пересекается с существующими
bool IsWallPositionFree(int wx, int wy, int ww, int wh, vector<Wall*>& walls)
{
    //wx, wy - координаты левого верхнего угла новой стены
    //ww, wh - ширина и высота новой стены
    //Создаем новый временный объект с данными аргументами
    Wall temp(wx, wy, ww, wh);
    //Перебираем все существующие стены
    for (int i = 0; i < (int)walls.size(); i++)
    {
        if (WallsCollide(&temp, walls[i]))
            return false;
    }
    return true;
}


int main()
{
    setlocale(LC_ALL, "RU");
    srand(time(0));

    //Получение дескриптора окна консоли
    HWND hwnd = GetConsoleWindow();
    if (!hwnd)
        return 1;  //Ошибка: окно консоли не найдено

    //Получение контекста устройства для рисования
    hdc = GetWindowDC(hwnd);
    if (!hdc)
        return 2;  //Ошибка: контекст устройства не получен

    //Открываем на весь экран
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);

    //Получаем размер окна
    RECT rect;
    GetClientRect(hwnd, &rect);
    screenWidth = rect.right;
    screenHeight = rect.bottom;

    //Настройка игрового поля
    fieldW = 800;
    fieldH = 600;
    fieldX = (screenWidth - fieldW) / 2;
    fieldY = (screenHeight - fieldH) / 2;
    COLORREF bodyColor = RGB(0, 100, 0);

    //Кисти и перья
    whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    borderBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    borderPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));

    //Создаем буфер
    InitBuffer();

    //Создание змейки
    SnakeHead* head = new SnakeHead(fieldX + fieldW / 2, fieldY + fieldH / 2);
    head->SetRadius(15);

    vector<SnakeBody*> body;
    SnakeBody* body1 = new SnakeBody(fieldX + fieldW / 2 - 30, fieldY + fieldH / 2);
    body1->SetRadius(15);
    body.push_back(body1);

    //Создание еды
    Circle* foodCircle = new Circle(0, 0);
    Triangle* foodTriangle = new Triangle(0, 0);
    foodCircle->SetRadius(12);
    foodTriangle->SetRadius(12);

    //Создаем случайные позиции для еды (врагов ещё нет)
    foodCircle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);
    foodTriangle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);

    //Счет игры
    int score = 0;
    int dirX = 1, dirY = 0;
    int oldDirX = 1, oldDirY = 0;
    int pendingGrowth = 0;
    int moveDelay = 0;

    //Враги
    vector<Enemy*> enemies;
    vector<Wall*> walls;
    int enemyCount = 0;
    int wallCount = 0;

    cin.get();

    //Отрисовка всего
    DrawAll(head, body, foodCircle, foodTriangle, enemies, walls);

    int level = 1;

    while (true)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        if (GetAsyncKeyState(VK_UP) & 0x8000 && oldDirY != 1) { dirX = 0; dirY = -1; }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && oldDirY != -1) { dirX = 0; dirY = 1; }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && oldDirX != 1) { dirX = -1; dirY = 0; }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && oldDirX != -1) { dirX = 1; dirY = 0; }

        moveDelay++;
        if (moveDelay >= 8)
        {
            moveDelay = 0;

            int oldHeadX = head->GetX();
            int oldHeadY = head->GetY();
            int newX = oldHeadX + dirX * 31;
            int newY = oldHeadY + dirY * 31;

            // Проверка границ 
            if (!CheckBoundary(newX, newY, head->GetRadius()))
            {
                cout << "Столкновение со стеной! Игра окончена!" << endl;
                break;
            }

            

            // Проверка столкновения со стенами (добавить)
            if (wallCount > 0 && CheckWallCollision(newX, newY, head->GetRadius(), walls))
            {
                cout << "Столкновение со стеной-препятствием! Игра окончена!" << endl;
                break;
            }

            //Движение тела
            for (int i = (int)body.size() - 1; i > 0; i--)
                body[i]->MoveTo(body[i - 1]->GetX(), body[i - 1]->GetY());
            body[0]->MoveTo(oldHeadX, oldHeadY);
            head->MoveTo(newX, newY);

            bool hit = false;

            //Движение и проверка врагов
            if (enemyCount > 0)
            {
                for (int i = 0; i < (int)enemies.size(); i++)
                {
                    enemies[i]->Move(fieldX + fieldW / 2, fieldY + fieldH / 2, fieldW / 2, walls);
                }

                // Проверка столкновения головы с врагами
                for (int i = 0; i < (int)enemies.size(); i++)
                {
                    int d = (head->GetRadius() + enemies[i]->GetRadius());
                    d *= d;
                    if (Dist2(head->GetX(), head->GetY(), enemies[i]->GetX(), enemies[i]->GetY()) <= d)
                    {
                        cout << "Столкновение головы с врагом! Игра окончена!" << endl;
                        hit = true;
                        break;
                    }
                }
                if (hit) break;

                // Проверка столкновения тела с врагами
                for (int i = 0; i < (int)body.size(); i++)
                {
                    for (int j = 0; j < (int)enemies.size(); j++)
                    {
                        int d = (body[i]->GetRadius() + enemies[j]->GetRadius());
                        d *= d;
                        if (Dist2(body[i]->GetX(), body[i]->GetY(), enemies[j]->GetX(), enemies[j]->GetY()) <= d)
                        {
                            cout << "Столкновение тела с врагом! Игра окончена!" << endl;
                            hit = true;
                            break;
                        }
                    }
                    if (hit) break;
                }
                if (hit) break;
            }

            // Проверка столкновения с телом
            for (int i = 0; i < (int)body.size(); i++)
            {
                int d = (head->GetRadius() + body[i]->GetRadius());
                d *= d;
                if (Dist2(head->GetX(), head->GetY(), body[i]->GetX(), body[i]->GetY()) <= d)
                {
                    hit = true;
                    break;
                }
            }
            if (hit)
            {
                cout << "Столкновение с телом! Игра окончена!" << endl;
                break;
            }

            //Добавление новых сегментов
            while (pendingGrowth > 0)
            {
                SnakeBody* newBody = new SnakeBody(oldHeadX, oldHeadY);
                newBody->SetRadius(15);
                newBody->SetFillColor(bodyColor);
                newBody->SetBorderColor(bodyColor);
                body.push_back(newBody);
                pendingGrowth--;
            }

            oldDirX = dirX;
            oldDirY = dirY;

            //Проверка поедания круга
            int dist = (head->GetRadius() + foodCircle->GetRadius());
            dist *= dist;
            if (Dist2(head->GetX(), head->GetY(), foodCircle->GetX(), foodCircle->GetY()) <= dist)
            {
                score++;
                pendingGrowth += 1;
                bodyColor = RGB(255, 0, 0);
                recolorBody(body, bodyColor);

                //Новая позиция для еды (не на врагах)
                int newXfood, newYfood;
                do {
                    newXfood = rand() % (fieldW - 100) + fieldX + 50;
                    newYfood = rand() % (fieldH - 100) + fieldY + 50;
                } while (!IsPositionFreeForFood(newXfood, newYfood, enemies, walls));
                foodCircle->MoveTo(newXfood, newYfood);
            }

            //Проверка поедания треугольника
            dist = (head->GetRadius() + foodTriangle->GetRadius());
            dist *= dist;
            if (Dist2(head->GetX(), head->GetY(), foodTriangle->GetX(), foodTriangle->GetY()) <= dist)
            {
                score += 2;
                pendingGrowth += 2;
                bodyColor = RGB(0, 0, 255);
                recolorBody(body, bodyColor);

                //Новая позиция для еды (не на врагах)
                int newXfood, newYfood;
                do {
                    newXfood = rand() % (fieldW - 100) + fieldX + 50;
                    newYfood = rand() % (fieldH - 100) + fieldY + 50;
                } while (!IsPositionFreeForFood(newXfood, newYfood, enemies, walls));
                foodTriangle->MoveTo(newXfood, newYfood);
            }

            //Появление врагов на 2 уровне
            if (score >= 5 && enemyCount == 0)
            {
                level = 2;
                DeleteObject(whiteBrush);
                whiteBrush = CreateSolidBrush(RGB(200, 220, 255));

                for (int i = 0; i < 5; i++)
                {
                    int ex, ey;
                    do {
                        ex = fieldX + rand() % fieldW;
                        ey = fieldY + rand() % fieldH;
                    } while (!IsPositionFreeForSpawn(ex, ey, 12, head, body, foodCircle, foodTriangle, enemies, walls));

                    Enemy* e = new Enemy(ex, ey);
                    e->SetRadius(12);
                    enemies.push_back(e);
                }
                enemyCount = (int)enemies.size();
            }

            //Появление стен на 3 уровне 
            if (score >= 10 && wallCount == 0)
            {
                wallCount = 1;

                for (int i = 0; i < 5; i++)
                {
                    int wx, wy, wtype, ww, wh;
                    bool bad;
                    do {
                        bad = false;
                        wx = fieldX + 20 + rand() % (fieldW - 100);
                        wy = fieldY + 20 + rand() % (fieldH - 100);
                        wtype = rand() % 2;  // только 0 или 1 

                        if (wtype == 0) { ww = 15; wh = 80; }   // вертикальная
                        else { ww = 80; wh = 15; }              // горизонтальная

                        //Проверка, что стена не выходит за границы поля
                        if (wx < fieldX || wx + ww > fieldX + fieldW ||
                            wy < fieldY || wy + wh > fieldY + fieldH)
                            bad = true;

                        //Проверка, что стена не пересекается с другими стенами
                        if (!bad && !IsWallPositionFree(wx, wy, ww, wh, walls))
                            bad = true;

                        //Проверка, что стена не на змее, еде, врагах
                        if (!bad)
                        {
                            if (!IsPositionFreeForSpawn(wx, wy, 0, head, body, foodCircle, foodTriangle, enemies, walls) ||
                                !IsPositionFreeForSpawn(wx + ww, wy, 0, head, body, foodCircle, foodTriangle, enemies, walls) ||
                                !IsPositionFreeForSpawn(wx, wy + wh, 0, head, body, foodCircle, foodTriangle, enemies, walls) ||
                                !IsPositionFreeForSpawn(wx + ww, wy + wh, 0, head, body, foodCircle, foodTriangle, enemies, walls))
                                bad = true;
                        }

                    } while (bad);

                    walls.push_back(new Wall(wx, wy, ww, wh));
                }

            }
        }

        //Перерисовка
        DrawAll(head, body, foodCircle, foodTriangle, enemies, walls);
        Sleep(16);
    }

    //Удаляем созданные объекты
    DeleteObject(bufferBitmap);
    DeleteDC(bufferDC);
    DeleteObject(whiteBrush);
    DeleteObject(borderPen);

    delete head;
    for (int i = 0; i < (int)body.size(); i++)
        delete body[i];
    delete foodCircle;
    delete foodTriangle;
    for (int i = 0; i < (int)enemies.size(); i++)
        delete enemies[i];

    for (int i = 0; i < (int)walls.size(); i++)
        delete walls[i];

    cout << "Ваш счет: " << score << endl << "Нажмите Enter для выхода..." << endl;
    cin.get();
    return 0;
}