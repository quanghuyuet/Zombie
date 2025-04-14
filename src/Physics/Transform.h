#pragma once
#include "Vector2D.h"
using namespace std;
class Transform
{
public:
    float X,Y;
    Transform(float x, float y)
   {
    this->X = x;
    this->Y = y;
   }
public:

    inline void TranslateX(float x) {X += x;}
    inline void TranslateY(float y) {Y += y;}
    inline void Translate(Vector2D v) { X+=v.X;Y+=v.Y;}

};
