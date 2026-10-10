#include "layout.hpp"

/*
fitSizingWidths();
grow&shrinkSizingWidghts();
wrapText();
fitSizingHeights();
grow&shrinkSizingHeights();
setPositions();
*/

Vec2 operator+(const Vec2& v1, const Vec2& v2){
    return Vec2{v1.x + v2.x, v1.y + v2.y};
}

Vec2& operator+=(Vec2& v1,const Vec2& v2){
    v1.x += v2.x;
    v1.y += v2.y;

    return v1;
}

Vec2 operator-(const Vec2& v1,const Vec2& v2){
    return Vec2{v1.x - v2.x, v1.y - v2.y};
}

std::ostream& operator<<(std::ostream& os,const Vec2& v){
    os << "x:" << v.x << ", y:" << v.y;
    return os;
}

void computeLayout(UIElement* root){
    for(auto& child: root->children){
        computeSize(child.get());
    }
}

void computeSize(UIElement* element){
    for(auto& child: element->children){
        computeSize(child.get());
    }
    if(element->parent->orientation == Orientation::Horizontal){
        element->parent->size.x += element->size.x;
        element->parent->size.y = std::max(element->parent->size.y,element->size.y);
    }
    else{
        element->parent->size.y += element->size.y;
        element->parent->size.x = std::max(element->parent->size.x,element->size.x);
    }
}