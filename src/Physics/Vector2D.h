#pragma once
#include <bits/stdc++.h>
using namespace std;
class Vector2D {
public:
    float X, Y;

    Vector2D(float x_ = 0, float y_ = 0) {
        this->X = x_;
        this->Y = y_;
    }

    Vector2D operator+(const Vector2D& other) {
        return Vector2D(X + other.X, Y + other.Y);
    }

    Vector2D operator-(const Vector2D& other) {
        return Vector2D(X - other.X, Y - other.Y);
    }

    Vector2D operator*(float scalar) const {
        return Vector2D(X * scalar, Y * scalar);
    }

    Vector2D& operator+=(const Vector2D& other) {
        X += other.X;
        Y += other.Y;
        return *this;
    }
};
