#include "UI.hpp"
#include <iostream> 

Vec2 operator+(const Vec2& v1, const Vec2& v2){
    return Vec2{v1.x + v2.x, v1.y + v2.y};
}

Vec2 operator-(const Vec2& v1,const Vec2& v2){
    return Vec2{v1.x - v2.x, v1.y - v2.y};
}

std::ostream& operator<<(std::ostream& os,const Vec2& v){
    os << "x:" << v.x << ", y:" << v.y;
    return os;
}

float magnitude(const Vec2& v1, const Vec2& v2){
    return std::sqrt(std::pow(v2.x - v1.x,2) + std::pow(v2.y - v1.y,2));
}

RectElement::RectElement(const Vec2& Apos,const Vec2& Asize,const Color& Acolor): 
                        pos(Apos), 
                        size(Asize), 
                        color(Acolor){};

TextElement::TextElement(const std::string& Atext,float Asize,const Vec2& Apos,const Color& Acolor):
                         text(Atext),
                         size(Asize),
                         pos(Apos),
                         color(Acolor){};

ButtonWidget::ButtonWidget(const ButtonParam& data){
    type = data.type;
    pos = data.pos;
    pos_offset = data.pos_offset;
    size = data.size;
    bg_color = data.bg_color;
    hover_color = data.hover_color;
    active_color = bg_color;
    text = data.text;
    text_color = data.text_color;
    text_size = data.text_size;
    font_id = data.font_id;
    padding = data.padding;
}

bool ButtonWidget::isHovering(const Vec2& mouse_pos) {
    float startX = pos.x + pos_offset.x;
    float startY = pos.y + pos_offset.y;

    return (mouse_pos.x >= startX && mouse_pos.x <= startX + size.x) &&
           (mouse_pos.y >= startY && mouse_pos.y <= startY + size.y);
}


void ButtonWidget::update(Vec2 mouse_pos,bool click){
    if(isHovering(mouse_pos)){
        active_color = hover_color;
        if (click){
            active_color = bg_color;
        }
    }
    else{
        active_color = bg_color;
    }
}

void ButtonWidget::draw(std::vector<Element>& buffer){
    buffer.emplace_back(std::in_place_type<RectElement>,pos + pos_offset,size,active_color);
    buffer.emplace_back(std::in_place_type<TextElement>,text,text_size,pos + Vec2{padding/2,padding/2},text_color);
}

Mouse::Mouse(){
    buttons = {
        {MouseButtonType::Left,MouseButton{false,false}},
        {MouseButtonType::Middle,MouseButton{false,false}},
        {MouseButtonType::Right,MouseButton{false,false}}
    };
}

void Mouse::update(const std::array<float,2>& pos){
    if(!(mouse_pos.x == INFINITY)){
        Vec2 prev_mouse_pos = mouse_pos;
        mouse_pos = Vec2{pos[0],pos[1]};
        drag = mouse_pos-prev_mouse_pos;
    }
    else{
        mouse_pos = Vec2{pos[0],pos[1]};
    }
}

void Mouse::updateLeftMouseButton(bool AisPressed){
    buttons[MouseButtonType::Left].wasPressed = buttons[MouseButtonType::Left].isPressed;
    buttons[MouseButtonType::Left].isPressed = AisPressed;
}

void Mouse::updateRightMouseButton(bool AisPressed){
    buttons[MouseButtonType::Right].wasPressed = buttons[MouseButtonType::Right].isPressed;
    buttons[MouseButtonType::Right].isPressed = AisPressed;
}

void Mouse::updateMiddleMouseButton(bool AisPressed){
    buttons[MouseButtonType::Middle].wasPressed = buttons[MouseButtonType::Middle].isPressed;
    buttons[MouseButtonType::Middle].isPressed = AisPressed;
}

bool Mouse::isClicked(MouseButtonType type){
    if(!(buttons[type].isPressed) && buttons[type].wasPressed){return true;}
    return false;
}

UI::UI(json& data,TextMeasurer m){
    measure = m;
    for(auto& element: data["ui"]){
        if(element["type"] == "Button"){
            ButtonParam param = parseButtonData(element);
            addButton(param);
        }
    }
};

void UI::update(){
    frameRenderBuffer.clear();
    for(auto& widget: widgets){
        widget->update(mouse.mouse_pos,mouse.isClicked(MouseButtonType::Left));
        widget->draw(frameRenderBuffer);
    }
}

ButtonParam UI::parseButtonData(json& data){
    ButtonParam param;
    BoundingBox bb = measure(data.at("text"),(int)data.at("font_id"),(float)data.at("text_size"));
    param.type = WidgetType::Button;
    param.pos = Vec2{(float)data.at("pos")[0],(float)data.at("pos")[1]};
    param.padding = data.at("padding");
    param.size = Vec2{bb.size.x + param.padding,bb.size.y + param.padding};
    param.pos_offset = Vec2{bb.pos.x,bb.pos.y};
    param.bg_color = Color{data.at("bg_color")[0],data.at("bg_color")[1],data.at("bg_color")[2]};
    param.hover_color = Color{data.at("hover_color")[0],data.at("hover_color")[1],data.at("hover_color")[2]};
    param.text = data.at("text");
    param.text_color = Color{data.at("text_color")[0],data.at("text_color")[1],data.at("text_color")[2]};
    param.text_size = data.at("text_size");
    param.font_id = data.at("font_id");

    return param;
};

void UI::addButton(ButtonParam& data){
    widgets.emplace_back(std::make_unique<ButtonWidget>(data));
}