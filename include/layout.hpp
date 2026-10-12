#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <array>

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

enum OrderDirection{
    Row,
    Col,
};

enum SizingMode{
    Fill,
    Fixed,
    Fit
};

struct Edge{
    float value;
    SizingMode mode;
    int grow;
    int shrink;
    float min;
    float max;
};

struct Padding{
    float left,right,top,bottom;
};

struct UIElement{
    Vec2 pos;
    std::string id;
    Edge width;
    Edge height;
    UIElement* parent;
    OrderDirection direction;
    Color bg_color;
    Padding padding;
    float child_gap;
    std::vector<std::unique_ptr<UIElement>> children;

    UIElement();

};

void computeSize(UIElement* element);
void growElement(UIElement* element);
void computeLayout(UIElement* root);
void computePosition(UIElement* root);