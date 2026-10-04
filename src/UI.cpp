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

bool rectPointCollision(const Vec2& rect_pos,const Vec2& rect_size,const Vec2& point){
    return (point.x >= rect_pos.x && point.x <= rect_pos.x + rect_size.x) &&
           (point.y >= rect_pos.y && point.y <= rect_pos.y + rect_size.y);
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

    return rectPointCollision(Vec2{startX, startY}, size, mouse_pos);
}


void ButtonWidget::update(Mouse& mouse){
    if(isHovering(mouse.mouse_pos)){
        active_color = hover_color;
        if (mouse.isClicked(MouseButtonType::Left)){
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

SliderWidget::SliderWidget(const SliderParam& data){
    type = data.type;
    track_pos = data.track_pos;
    track_size = data.track_size;
    handle_size = data.handle_size;
    range = data.range;
    value = data.value;
    step = data.step;
    orientation = data.orientation;
    track_color = data.track_color;
    handle_base_color = data.handle_base_color;
    handle_hover_color = data.handle_hover_color;
    handle_active_color = data.handle_base_color;

    if(orientation){
        handle_pos = Vec2{track_pos.x + (value-range.x)/(range.y-range.x)*(track_size.x-handle_size.x),track_pos.y};
    }
    else{
        handle_pos = Vec2{track_pos.x,track_pos.y + (value-range.x)/(range.y-range.x)*(track_size.y-handle_size.y)};
    }
}

bool SliderWidget::isHovering(const Vec2& mouse_pos){
    return rectPointCollision(handle_pos, handle_size, mouse_pos);
}

void SliderWidget::update(Mouse& mouse){
    if(isHovering(mouse.mouse_pos)){
        handle_active_color = handle_hover_color;
        if(mouse.buttons[MouseButtonType::Left].isPressed){
            selected = true;
        }
    }
    if(!(mouse.buttons[MouseButtonType::Left].isPressed)){
        selected = false;
    }
    if(!isHovering(mouse.mouse_pos) && !selected){
        handle_active_color = handle_base_color;
    }
    if(selected && mouse.isDragging(MouseButtonType::Left)){
        if(orientation){
            handle_pos.x = std::clamp(handle_pos.x + mouse.drag.x, track_pos.x, track_pos.x + (track_size.x - handle_size.x));
            value = std::clamp(range.x + (std::round(((handle_pos.x - track_pos.x)/(track_size.x - handle_size.x) * (range.y - range.x))/step) * step), range.x , range.y);
        }
        else{
            handle_pos.y = std::clamp(handle_pos.y + mouse.drag.y, track_pos.y, track_pos.y + (track_size.y - handle_size.y));
            value = std::clamp(range.x + (std::round(((handle_pos.y - track_pos.y)/(track_size.y - handle_size.y) * (range.y - range.x))/step) * step), range.x , range.y);
        }
    }
}

void SliderWidget::draw(std::vector<Element>& buffer){
    buffer.emplace_back(std::in_place_type<RectElement>,track_pos,track_size,track_color);
    buffer.emplace_back(std::in_place_type<RectElement>,handle_pos,handle_size,handle_active_color);
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

bool Mouse::isDragging(MouseButtonType type){
    if(buttons[type].isPressed && buttons[type].wasPressed & magnitude(mouse_pos,mouse_pos-drag) > drag_threshold){
        return true;
    }
    return false;
}

UI::UI(json& data,TextMeasurer m){
    measure = m;
    for(auto& element: data["ui"]){
        if(element["type"] == "Button"){
            widgets.emplace_back(std::make_unique<ButtonWidget>(parseButtonData(element)));
        }
        else if(element["type"] == "Slider"){
            widgets.emplace_back(std::make_unique<SliderWidget>(parseSliderData(element)));
        }
    }
};

void UI::update(){
    frame_render_buffer.clear();
    for(auto& widget: widgets){
        widget->update(mouse);
        widget->draw(frame_render_buffer);
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

SliderParam UI::parseSliderData(json& data){
    SliderParam param;
    param.type = WidgetType::Slider;
    param.track_pos = Vec2{data.at("pos")[0],data.at("pos")[1]};
    if(data.at("orientation") == "horizontal"){
        param.orientation = true;
    }
    else{
        param.orientation = false;
    }
    param.track_size = Vec2{data.at("track_size")[0],data.at("track_size")[1]};
    param.handle_size = Vec2{data.at("handle_size")[0],data.at("handle_size")[1]};
    param.range = Vec2{data.at("range")[0],data.at("range")[1]};
    param.value = data.at("value");
    param.step = data.at("step");
    param.track_color = Color{data.at("track_color")[0],data.at("track_color")[1],data.at("track_color")[2]};
    param.handle_base_color = Color{data.at("handle_base_color")[0],data.at("handle_base_color")[1],data.at("handle_base_color")[2]};
    param.handle_hover_color = Color{data.at("handle_hover_color")[0],data.at("handle_hover_color")[1],data.at("handle_hover_color")[2]};
    return param;
}