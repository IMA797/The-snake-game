#pragma once
#include<windows.h>
#include<vector>

class IDrawable
{
public:
    virtual void Show() = 0;
    virtual void Hide() = 0;

};

class Location
{
protected:
    int x;
    int y;
public:
    Location(int InitX, int InitY);
    ~Location();
    int GetX();
    int GetY();
    void SetX(int NewX);
    void SetY(int newY);
};

class Point : public Location
{
protected:
    bool visible;
    int radius;
public:
    Point(int InitX, int InitY);
    ~Point();
    virtual void MoveTo(int NewX, int NewY);
    void SetRadius(int r);
    virtual void DrawToDC(HDC targetDC) = 0;

    int GetRadius() { return radius; }
};

class Figure : public Point, public IDrawable
{
protected:
    //Граница фигуры
    COLORREF borderColor;
    //Заливка фигуры
    COLORREF fillColor;
    int lineWidth;
public:
    Figure(int InitX, int InitY);
    ~Figure();
    void SetBorderColor(COLORREF color) { borderColor = color; }
    void SetFillColor(COLORREF color) { fillColor = color; }
    void SetLineWidth(int width) { lineWidth = width; }
    COLORREF GetBorderColor() { return borderColor; }
    COLORREF GetFillColor() { return fillColor; }
    int GetLineWidth() { return lineWidth; }
};

class Circle : public Figure
{
public:
    Circle(int InitX, int InitY);
    ~Circle();
    void Show() override;
    void Hide() override;

    void DrawToDC(HDC targetDC) override;
};

class Triangle : public Figure
{
public:
    Triangle(int InitX, int InitY);
    ~Triangle();
    void Show() override;
    void Hide() override;

    void DrawToDC(HDC targetDC) override;
};

class Player : public Point, public IDrawable
{
protected:
    COLORREF borderColor;
    COLORREF fillColor;
    int lineWidth;
public:
    Player(int InitX, int InitY);
    ~Player();

    void SetBorderColor(COLORREF color) { borderColor = color; }
    void SetFillColor(COLORREF color) { fillColor = color; }
    void SetLineWidth(int width) { lineWidth = width; }
    COLORREF GetBorderColor() { return borderColor; }
    COLORREF GetFillColor() { return fillColor; }
    int GetLineWidth() { return lineWidth; }
};

class SnakeHead : public Player
{
public:
    SnakeHead(int InitX, int InitY);
    ~SnakeHead();
    void Show() override;
    void Hide() override;

    void DrawToDC(HDC targetDC) override;
};

class SnakeBody : public Player
{
public:
    SnakeBody(int InitX, int InitY);
    ~SnakeBody();
    void Show() override;
    void Hide() override;

    void DrawToDC(HDC targetDC) override;
};


class Wall : public Figure
{
public:
    Wall(int InitX, int InitY, int width, int height);
    ~Wall();
    void Show() override;
    void Hide() override;
    void DrawToDC(HDC targetDC) override;
    int GetW() { return w; }
    int GetH() { return h; }

private:
    int w, h;
};

class Enemy : public Figure
{
public:
    Enemy(int InitX, int InitY);
    ~Enemy();
    void Show() override;          
    void Hide() override;
    void DrawToDC(HDC targetDC);    
    void Move(int centerX, int centerY, int fieldRadius, std::vector<Wall*>& walls);
};
