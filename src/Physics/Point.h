
#pragma once
#include <iostream>
using namespace std;
class Point {
public:
    float X, Y;
    Point(float x_ = 0, float y_ = 0){
    this->X = x_;
    this->Y = y_;
    }
   inline Point operator+(const Point& other) const {
        return Point(X + other.X, Y + other.Y);
    }

    inline Point operator-(const Point& other) const {
            return Point(X - other.X, Y - other.Y);
        }
    inline Point operator*(float scalar) const {
        return Point(X * scalar, Y * scalar);
    }
    inline Point friend operator-=(Point& p1, const Point &p2)
    {
        p1.X -= p2.X;
        p1.Y -= p2.Y;
        return p1;
    }
      inline Point friend operator+=(Point& p1, const Point &p2)
    {
        p1.X += p2.X;
        p1.Y += p2.Y;
        return p1;
    }
};
