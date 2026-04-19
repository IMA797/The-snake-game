#include "snake.h"
#include <windows.h>
#include <vector>

extern HDC hdc;

//start Location::Location
Location::Location(int InitX, int InitY)
{
    x = InitX;
    y = InitY;
}//end Location::Location

//start Location::~Location
Location::~Location()
{
}//end Location::~Location

//start Location::GetX
int Location::GetX()
{
    return x;
}//end Location::GetX

//start Location::GetY
int Location::GetY()
{
    return y;
}//end Location::GetY

//start Location::SetX
void Location::SetX(int NewX)
{
    x = NewX;
}//end Location::SetX

//start Location::SetY
void Location::SetY(int NewY)
{
    y = NewY;
}//end Location::SetY

//start Point::Point
Point::Point(int InitX, int InitY) : Location(InitX, InitY)
{
    //По умолчанию объект невидим и базовый радиус равен 10
    visible = false;
    radius = 10;
}//end Point::Point

//start Point::~Point
Point::~Point()
{
}//end Point::~Point

//start Point::SetRadius
void Point::SetRadius(int r)
{
    radius = r;
}//end Point::SetRadius

//start Point::MoveTo
void Point::MoveTo(int NewX, int NewY)
{
    x = NewX;
    y = NewY;
}
//end Point::MoveTo

//start Figure::Figure
Figure::Figure(int InitX, int InitY) : Point(InitX, InitY)
{
    //Цвет границы, заливки, и длина линии
    borderColor = RGB(255, 255, 255);
    fillColor = RGB(255, 255, 255);
    lineWidth = 2;
}//end Figure::Figure

//start Figure::~Figure
Figure::~Figure()
{
}//end Figure::~Figure

//start Circle::Circle
Circle::Circle(int InitX, int InitY) : Figure(InitX, InitY)
{
    //Устанавливаем цвет границы
    SetBorderColor(RGB(0, 255, 0));
    //Устанавливаем цвет заливки
    SetFillColor(RGB(0, 255, 0));
}//end Circle::Circle

//start Circle::~Circle
Circle::~Circle()
{
}//end Circle::~Circle

//start Circle::Show
//targetDC - то, куда будем рисовать
void Circle::Show()
{
    //Говорим показать наш объект, изменяя флаг на true
    visible = true;

    //Создаем перо и кисть
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    //Активируем созданные перо и кисть, при этом сохраняя их для дальнейшего вызова
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    //Рисуем эллипс
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    //Восстанавливаем старые инструменты
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    //Удаляем созданные инструменты
    DeleteObject(pen);
    DeleteObject(brush);
}//end Circle::Show

//start Circle::Hide
void Circle::Hide()
{
    //Если фигура не видно - return 
    if (!visible) return;

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);

    visible = false;
}//end Circle::Hide

//start Circle::DrawToDC
//targetDC - то, куда будем рисовать
void Circle::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    Ellipse(targetDC, x - radius, y - radius, x + radius, y + radius);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}
//end Circle::DrawToDC

//start Triangle::Triangle
Triangle::Triangle(int InitX, int InitY) : Figure(InitX, InitY)
{
    //Устанавливаем цвет границы
    SetBorderColor(RGB(0, 0, 255));
    //Устанавливаем цвет заливки
    SetFillColor(RGB(0, 0, 255));
}//end Triangle::Triangle

//start Triangle::~Triangle
Triangle::~Triangle()
{
}//end Triangle::~Triangle

//start Triangle::Show
void Triangle::Show()
{
    visible = true;

    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    //Определяем три вершины треугольника: верхняя, левая и правая
    POINT points[3] = {
        {x, y - radius},
        {x - radius, y + radius},
        {x + radius, y + radius}
    };
    //Рисуем закрашенный многоугольник по трем точкам
    Polygon(hdc, points, 3);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end Triangle::Show

//start Triangle::Hide
void Triangle::Hide()
{
    if (!visible) return;

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    POINT points[3] = {
        {x, y - radius},
        {x - radius, y + radius},
        {x + radius, y + radius}
    };
    Polygon(hdc, points, 3);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);

    visible = false;
}//end Triangle::Hide

//start Triangle::DrawToDC
void Triangle::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    POINT points[3] = {
        {x, y - radius},
        {x - radius, y + radius},
        {x + radius, y + radius}
    };
    Polygon(targetDC, points, 3);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}
//end Triangle::DrawToDC

//start Player::Player
Player::Player(int InitX, int InitY) : Point(InitX, InitY)
{
}//end Player::Player

//start Player::~Player
Player::~Player()
{
}//end Player::~Player

//start SnakeHead::SnakeHead
SnakeHead::SnakeHead(int InitX, int InitY) : Player(InitX, InitY)
{
}//end SnakeHead::SnakeHead

//start SnakeHead::~SnakeHead
SnakeHead::~SnakeHead()
{
}//end SnakeHead::~SnakeHead

//start SnakeHead::Show
void SnakeHead::Show()
{
    visible = true;

    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Rectangle(hdc, x - radius, y - radius, x + radius, y + radius);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end SnakeHead::Show

//start SnakeHead::Hide
void SnakeHead::Hide()
{
    if (!visible) return;

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Rectangle(hdc, x - radius, y - radius, x + radius, y + radius);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);

    visible = false;
}//end Snake::Head

//start SnakeHead::DrawToDC
void SnakeHead::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    Rectangle(targetDC, x - radius, y - radius, x + radius, y + radius);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}
//end SnakeHead::DrawToDC

//start SnakeBody::SnakeBody
SnakeBody::SnakeBody(int InitX, int InitY) : Player(InitX, InitY)
{
    borderColor = RGB(0, 100, 0);
    fillColor = RGB(0, 100, 0);
    lineWidth = 2;
}//end SnakeBody::SnakeBody

//start SnakeBody::~SnakeBody
SnakeBody::~SnakeBody()
{
}//end SnakeBody::~SnakeBody

//start SnakeBody::Show
void SnakeBody::Show()
{
    visible = true;

    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);

    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Rectangle(hdc, x - radius, y - radius, x + radius, y + radius);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(pen);
    DeleteObject(brush);
}//end SnakeBody::Show

//start SnakeBody::Hide
void SnakeBody::Hide()
{
    if (!visible) return;

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Rectangle(hdc, x - radius, y - radius, x + radius, y + radius);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);

    visible = false;
}//end SnakeBody::Hide

//start SnakeBody::DrawToDC
void SnakeBody::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    Rectangle(targetDC, x - radius, y - radius, x + radius, y + radius);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}
//end SnakeBody::DrawToDC

//start Enemy::Enemy
Enemy::Enemy(int InitX, int InitY) : Figure(InitX, InitY)
{
    borderColor = RGB(255, 0, 0);
    fillColor = RGB(255, 0, 0);
    lineWidth = 2;
    SetRadius(12);
}//end Enemy::Enemy

//start Enemy::~Enemy
Enemy::~Enemy()
{

}//end Enemy::~Enemy

//start Enemy::Show
void Enemy::Show()
{
    visible = true;
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end Enemy::Show

//start Enemy::Hide
void Enemy::Hide()
{
    if (!visible) return;
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
    visible = false;
}//end Enemy::Hide

//start Enemy::DrawToDC
void Enemy::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    Ellipse(targetDC, x - radius, y - radius, x + radius, y + radius);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end DrawToDC

// Проверка, не столкнулся ли враг со стеной
//start CheckEnemyWallCollision
bool CheckEnemyWallCollision(int newX, int newY, int radius, std::vector<Wall*>& walls)
{
    for (int i = 0; i < (int)walls.size(); i++)
    {
        if (newX + radius > walls[i]->GetX() && newX - radius < walls[i]->GetX() + walls[i]->GetW() &&
            newY + radius > walls[i]->GetY() && newY - radius < walls[i]->GetY() + walls[i]->GetH())
        {
            return true;
        }
    }
    return false;
}//end CheckEnemyWallCollision

//start Enemy::Move
void Enemy::Move(int centerX, int centerY, int fieldRadius, std::vector<Wall*>& walls)
{
    int newX = GetX();
    int newY = GetY();
    int dir = rand() % 8;

    switch (dir)
    {
    case 0: newX += 5; break;
    case 1: newX -= 5; break;
    case 2: newY += 5; break;
    case 3: newY -= 5; break;
    case 4: newX += 5; newY += 5; break;
    case 5: newX += 5; newY -= 5; break;
    case 6: newX -= 5; newY += 5; break;
    case 7: newX -= 5; newY -= 5; break;
    }

    // Принудительно удерживаем врага в пределах поля
    int radius = GetRadius();
    int leftBound = centerX - fieldRadius + radius;
    int rightBound = centerX + fieldRadius - radius;
    int topBound = centerY - fieldRadius + radius;
    int bottomBound = centerY + fieldRadius - radius;

    if (newX < leftBound) newX = leftBound;
    if (newX > rightBound) newX = rightBound;
    if (newY < topBound) newY = topBound;
    if (newY > bottomBound) newY = bottomBound;

    // Проверка столкновения со стенами
    bool validMove = !CheckEnemyWallCollision(newX, newY, radius, walls);

    if (validMove)
    {
        Hide();
        MoveTo(newX, newY);
        Show();
    }
}//end Enemy::Move

//start Wall::Wall
Wall::Wall(int InitX, int InitY, int width, int height) : Figure(InitX, InitY)
{
    borderColor = RGB(100, 100, 100);
    fillColor = RGB(100, 100, 100);
    lineWidth = 2;
    w = width;
    h = height;
    SetRadius(0);
}//end Wall::Wall

//start Wall::~Wall
Wall::~Wall() 
{
}//end Wall::~Wall

//start Wall::Show
void Wall::Show()
{
    visible = true;
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    Rectangle(hdc, x, y, x + w, y + h);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end Wall::Show

//start Wall::Hide
void Wall::Hide()
{
    if (!visible) return;
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    Rectangle(hdc, x, y, x + w, y + h);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
    visible = false;
}//end Wall::Hide

//start Wall::DrawToDC
void Wall::DrawToDC(HDC targetDC)
{
    HPEN pen = CreatePen(PS_SOLID, lineWidth, borderColor);
    HBRUSH brush = CreateSolidBrush(fillColor);
    HPEN oldPen = (HPEN)SelectObject(targetDC, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(targetDC, brush);
    Rectangle(targetDC, x, y, x + w, y + h);
    SelectObject(targetDC, oldPen);
    SelectObject(targetDC, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}//end Wall::DrawToDC
