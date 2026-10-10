#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

struct Vec2{
    float x,y;
};

Vec2 operator+(const Vec2& v1, const Vec2& v2);
Vec2& operator+=(Vec2& v1,const Vec2& v2);
Vec2 operator-(const Vec2& v1,const Vec2& v2);
std::ostream& operator<<(std::ostream& os,const Vec2& v);

struct Color{
    std::uint8_t r,g,b;
};

struct BoundingBox{
    Vec2 pos, size;
};

enum Orientation{
    Horizontal,
    Vertical,
};

struct UIElement{
    Vec2 pos;
    Vec2 size;
    UIElement* parent;
    Orientation orientation;
    std::vector<std::unique_ptr<UIElement>> children;
};

void computeLayout(UIElement* root);
void computeSize(UIElement* element);