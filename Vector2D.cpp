#include "Vector2D.hpp"

using namespace std;

Vector2D::Vector2D() {
    x = 0.0f;
    y = 0.0f;
}

Vector2D::Vector2D(float x, float y) {
    this -> x = x; // this -> : là thành viên của class
    this -> y = y;
}

Vector2D& Vector2D::Add(const Vector2D& vec) {
    this -> x += vec.x;
    this -> y += vec.y;
    return *this;
}

Vector2D& Vector2D::Subtract(const Vector2D& vec) {
    this -> x -= vec.x;
    this -> y -= vec.y;
    return *this;
}

Vector2D& Vector2D::Multiply(const Vector2D& vec) {
    this -> x *= vec.x;
    this -> y *= vec.y;
    return *this;
}

Vector2D& Vector2D::Devide(const Vector2D& vec) {
    this -> x /= vec.x;
    this -> y /= vec.y;
    return *this;
}

Vector2D& Vector2D::operator += (const Vector2D& vec) {
    return this -> Add(vec);
}

Vector2D& Vector2D::operator -= (const Vector2D& vec) {
    return this -> Subtract(vec);
}

Vector2D& Vector2D::operator *= (const Vector2D& vec) {
    return this -> Multiply(vec);
}

Vector2D& Vector2D::operator /= (const Vector2D& vec) {
    return this -> Devide(vec);
}

Vector2D& Vector2D::operator*(float i) {
    this -> x *= i;
    this -> y *= i;
    return *this;
}

Vector2D& Vector2D::Zero() {
    this -> x = 0;
    this -> y = 0;
    return *this;
}

ostream& operator << (ostream& stream, const Vector2D& vec) {
    stream << "(" << vec.x << "," << vec.y << ")";
    return stream;
}
