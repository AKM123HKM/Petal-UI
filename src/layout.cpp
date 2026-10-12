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

UIElement::UIElement(){
    pos = Vec2{0,0};
}

void computeSize(UIElement* element){
    for (auto& child: element->children){
        computeSize(child.get());
    }

    float width = 0.0f, height = 0.0f;
    for (auto& child: element->children){
        if (element->width.mode == SizingMode::Fit){
            if(element->direction == OrderDirection::Row){
                // element->width.value += child->width.value;
                width += child->width.value;
            }
            else{
                // element->width.value = std::max(child->width.value, element->width.value);
                width = std::max(child->width.value,width);
            }
        }
        if (element->height.mode == SizingMode::Fit){
            if (element->direction == OrderDirection::Col){
                // element->height.value += child->height.value;
                height += child->height.value;
            }
            else{
                // element->height.value = std::max(child->height.value, element->height.value);
                height = std::max(child->height.value, height);
            }
        }
        
        if (child->width.mode == SizingMode::Fill){
            child->width.value = 0.0f;
        }
        if (child->height.mode == SizingMode::Fill){
            child->height.value = 0.0f;
        }
    }

    if (element->width.mode == SizingMode::Fit){
        // element->width.value += element->padding.left + element->padding.right;
        width += element->padding.left + element->padding.right;
        if (element->direction == OrderDirection::Row){
            // element->width.value += (element->children.size() - 1) * element->child_gap;
            width += (element->children.size() - 1) * element->child_gap;
        }
    }

    if (element->height.mode == SizingMode::Fit){
        // element->height.value += element->padding.top + element->padding.bottom;
        height += element->padding.top + element->padding.bottom;
        if (element->direction == OrderDirection::Col){
            // element->height.value += (element->children.size() - 1) * element->child_gap;
            height += (element->children.size() - 1) * element->child_gap;
        }
    }

    if (element->width.mode == SizingMode::Fit){
        element->width.value = width;
    }
    if (element->height.mode == SizingMode::Fit){
        element->height.value = height;
    }
}

void growElement(UIElement* element){
    float remaining_width = element->width.value;
    float remaining_height = element->height.value;
    int net_width_grow = 0;
    int net_height_grow = 0;
    

    for (auto& child: element->children){
        if (element->direction == OrderDirection::Row){
            remaining_width -= child->width.value;
        }
        else{
            remaining_height -= child->height.value;
        }
        net_width_grow += child->width.grow;
        net_height_grow += child->height.grow;
    }

    
    if (element->direction == OrderDirection::Row){
        remaining_width -= (element->children.size() - 1) * element->child_gap;
    }
    else{
        remaining_height -= (element->children.size() - 1) * element->child_gap;
    }
    
    remaining_width -= element->padding.left + element->padding.right;
    remaining_height -= element->padding.top + element->padding.bottom;
    
    
    for (auto& child: element->children){
        if (child->width.grow){
            if (element->direction == OrderDirection::Col){
                child->width.value += remaining_width - child->width.value;
            }
            else{
                child->width.value += remaining_width * (child->width.grow / net_width_grow);
            }
        }
        if (child->height.grow){
            if (element->direction == OrderDirection::Row){
                child->height.value += remaining_height - child->height.value;
            }
            else{
                child->height.value += remaining_height * (child->height.grow / net_height_grow);
            }
        }
    }

    for (auto& child: element->children){
        growElement(child.get());
    }
}

void computePosition(UIElement* root){
    Vec2 child_pos = Vec2{root->padding.left,root->padding.top};
    for(auto& child: root->children){
        child->pos = child_pos + root->pos;
        if(root->direction == OrderDirection::Row){
            child_pos.x += child->width.value + root->child_gap;
        }
        else{
            child_pos.y += child->height.value + root->child_gap;
        }
    }
    for (auto& child: root->children){
        computePosition(child.get());
    }
}

void computeLayout(UIElement* root){
    computeSize(root);
    growElement(root);
    computePosition(root);
}