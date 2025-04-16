#pragma once
#include <iostream>

using namespace std;

class Vector2D {
public:
    float x;
    float y;

    Vector2D();
    Vector2D(float x, float y);

    Vector2D& Add(const Vector2D& vec);
    Vector2D& Subtract(const Vector2D& vec);
    Vector2D& Multiply(const Vector2D& vec);
    Vector2D& Devide(const Vector2D& vec);

    friend Vector2D& operator + (Vector2D& v1, const Vector2D& v2);
    friend Vector2D& operator - (Vector2D& v1, const Vector2D& v2);
    friend Vector2D& operator * (Vector2D& v1, const Vector2D& v2);
    friend Vector2D& operator / (Vector2D& v1, const Vector2D& v2);

    Vector2D& operator += (const Vector2D& vec);
    Vector2D& operator -= (const Vector2D& vec);
    Vector2D& operator *= (const Vector2D& vec);
    Vector2D& operator /= (const Vector2D& vec);

    Vector2D& operator*(const int& i);
    Vector2D& Zero();

    friend ostream& operator << (ostream& stream, const Vector2D& vec); // friend cho phép truy cập vào private của class
};
