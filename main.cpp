/******************************************************************
*                 Л е т н я я   п р а к т и к а                   *
*-----------------------------------------------------------------*
* Project Type  : Console Application                             *
* Project Name  : Praktika_zadanie_3                              *
* File Name     : main.cpp                                        *
* Language      : C++                                             *
* Programmer    : Исаев Магомед Абдурахманович                    *
* Created       : 29/03/26                                        *
* Last Revision : 4/04/26                                         *
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
    //В аргументе: координаты левого верхнего угла, и нижнего правого угла
    Rectangle(bufferDC, fieldX, fieldY, fieldX + fieldW, fieldY + fieldH);
}

//Отрисовка игры
void DrawAll(SnakeHead* head, vector<SnakeBody*>& body, Circle* foodCircle, Triangle* foodTriangle)
{
    //Очистка экрана, то есть заливка его большим прямоугольником 
    SelectObject(bufferDC, whiteBrush);
    Rectangle(bufferDC, -500, -500, screenWidth + 500, screenHeight + 500);

    //Рисуем рамку экрана
    DrawField();

    /*Тело змейки
    Вначале рисуется тело, а потом голова
    Если рисовать наоборот, то тело может быть свеху головы*/
    for (int i = (int)body.size() - 1; i >= 0; i--)
        body[i]->DrawToDC(bufferDC);

    //Рисуем голову
    head->DrawToDC(bufferDC);

    //Рисуем еду 
    foodCircle->DrawToDC(bufferDC);
    foodTriangle->DrawToDC(bufferDC);

    /*Перенос на экран
    hdc - дескриптор контекста экрана
    0, 0 - левый верхний угол экрана
    screenWidth - ширина экрана
    screenHeight - высота экрана
    bufferDC - конттекст буфера
    0, 0 - координаты в буфере
    SRCCOPY - операция копирования*/
    BitBlt(hdc, 0, 0, screenWidth, screenHeight, bufferDC, 0, 0, SRCCOPY);
}

//Проверка выхода за игровое поле
bool CheckBoundary(int x, int y, int radius)
{
    return (x - radius >= fieldX && x + radius <= fieldX + fieldW &&
        y - radius >= fieldY && y + radius <= fieldY + fieldH);
}

void recolorBody(vector<SnakeBody*>& body, COLORREF newColor)
{
    for (int i = 0; i < (int)body.size(); i++)
    {
        body[i]->SetFillColor(newColor);
        body[i]->SetBorderColor(newColor);
    }
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

    //Открываем на весь экран (SW_SHOWMAXIMIZED - команда "развернуть на весь экран)
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);

    /*Получаем размер полученного окна (сначала открываем на весь экран, а потом получаем размер): ширина и высота
    rect - структура, которая описывает прямоугольник.
    В нем хранится 4 числа: координаты левого, правого, верхнего и нижнего краев
    GetClientRect - заполняем структуру rect текущими размерами окна
    Нужно для создания буфера под размер окна*/
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

    //Кисти и перья, необходимые для рисования: 
    //Белая кисть, пустая кисть и черное перо
    whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    borderBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    borderPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));

    //Создаем буфер
    InitBuffer();

    //Создание змейки: голова одна (по центру поля), тело - хранится в векторе (вначале отрисовывается слева от головы)
    SnakeHead* head = new SnakeHead(fieldX + fieldW / 2, fieldY + fieldH / 2);
    head->SetRadius(15);

    //Вектор указателей на тело (SnakeBody)
    vector<SnakeBody*> body;
    //Тело смещено от центра на 30 
    SnakeBody* body1 = new SnakeBody(fieldX + fieldW / 2 - 30, fieldY + fieldH / 2);
    body1->SetRadius(15);
    body.push_back(body1);

    //Создание еды: круга и треугольника
    Circle* foodCircle = new Circle(0, 0);
    Triangle* foodTriangle = new Triangle(0, 0);
    foodCircle->SetRadius(12);
    foodTriangle->SetRadius(12);

    //Создаем случайные позиции для еды
    foodCircle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);
    foodTriangle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);

    //Счет игры
    int score = 0;
    //Текущее направление (вправо)
    int dirX = 1, dirY = 0;
    //Прошлое направление
    int oldDirX = 1, oldDirY = 0;
    //Сколько сегментов добавляется
    int pendingGrowth = 0;
    //Нужно для задержки движения
    int moveDelay = 0;

    cin.get();

    //Отрисовка всего
    DrawAll(head, body, foodCircle, foodTriangle);

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

            //Сохраняем старые позиции головы
            int oldHeadX = head->GetX();
            int oldHeadY = head->GetY();
            //Новые координаты: шаг - 15 пикселей
            int newX = oldHeadX + dirX * 31;
            int newY = oldHeadY + dirY * 31;

            // Проверка границ 
            if (!CheckBoundary(newX, newY, head->GetRadius()))
            {
                cout << "Столкновение со стеной! Игра окончена!" << endl;
                break;
            }

            //Движение тела
            for (int i = (int)body.size() - 1; i > 0; i--)
                //каждый новый сегмент во время движения становится на место предыдущего 
                body[i]->MoveTo(body[i - 1]->GetX(), body[i - 1]->GetY());
            body[0]->MoveTo(oldHeadX, oldHeadY);
            //Движение головы
            head->MoveTo(newX, newY);

            //Флаг столкновения с телом (true - есть столкновение, false - иначе)
            bool hit = false;

            //Проходимся по всем сегментам тела
            for (int i = 0; i < (int)body.size(); i++)
            {
                //Расстояние, при котором голова и сегмент касаются друг друга (квадрат суммы радиусов)
                int d = (head->GetRadius() + body[i]->GetRadius());
                d *= d;
                //Сравнение квадрат реального расстояния с квадратом суммы радиусов
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

            //Цикл добавления нового сегмента тела
            while (pendingGrowth > 0)
            {
                SnakeBody* newBody = new SnakeBody(oldHeadX, oldHeadY);
                newBody->SetRadius(15);
                newBody->SetFillColor(bodyColor);
                newBody->SetBorderColor(bodyColor);
                body.push_back(newBody);
                pendingGrowth--;
            }

            //Запоминаем направления для следующего шага
            oldDirX = dirX;
            oldDirY = dirY;

            //Проверка поедания круга (он добавляет 1 сегмент)
            int dist = (head->GetRadius() + foodCircle->GetRadius());
            dist *= dist;
            if (Dist2(head->GetX(), head->GetY(), foodCircle->GetX(), foodCircle->GetY()) <= dist)
            {
                score++;
                pendingGrowth += 1;
                bodyColor = RGB(255, 0, 0);
                recolorBody(body, bodyColor);
                foodCircle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);
            }

            //Проверка поедания треугольника (он добавляет 2 сегмента)
            dist = (head->GetRadius() + foodTriangle->GetRadius());
            dist *= dist;
            if (Dist2(head->GetX(), head->GetY(), foodTriangle->GetX(), foodTriangle->GetY()) <= dist)
            {
                score += 2;
                pendingGrowth += 2;
                bodyColor = RGB(0, 0, 255);
                recolorBody(body, bodyColor);
                foodTriangle->MoveTo(rand() % (fieldW - 100) + fieldX + 50, rand() % (fieldH - 100) + fieldY + 50);
            }
        }

        //Перерисовываем все
        DrawAll(head, body, foodCircle, foodTriangle);
        Sleep(16);
    }

    //Удаляем созданные объекты
    DeleteObject(bufferBitmap);
    DeleteDC(bufferDC);
    DeleteObject(whiteBrush);
    DeleteObject(borderPen);

    //Удаляем голову, тело, еду
    delete head;
    for (int i = 0; i < (int)body.size(); i++)
    {
        delete body[i];
    }
    delete foodCircle;
    delete foodTriangle;

    cout << "Ваш счет: " << score << endl << "Нажмите Enter для выхода..." << endl;
    cin.get();
    return 0;
}
