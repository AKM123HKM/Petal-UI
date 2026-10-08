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

std::ostream& operator<<(std::ostream& os,WidgetType type){
    switch(type){
        case WidgetType::Rectangle:
            os << "Rectangle";
            break;
        case WidgetType::Label:
            os << "Label";
            break;
        case WidgetType::Button:
            os << "Button";
            break;
        case WidgetType::Slider:
            os << "Slider";
            break;
    }
    return os;
}

Color getColor(const json& data){
    return Color{data[0],data[1],data[2]};
}

Vec2 getVec2(const json& data){
    return Vec2{data[0],data[1]};
}

float distance(const Vec2& v1, const Vec2& v2){
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

#pragma region RectWidget

RectWidget::RectWidget(const RectParam& data){
    pos = data.pos;
    size = data.size;
    base_color = data.base_color;
    hover_color = data.hover_color;
    active_color = base_color;
    type = WidgetType::Rectangle;
}

void RectWidget::update(Mouse& mouse){
    if(rectPointCollision(pos, size, mouse.mouse_pos)){
        active_color = hover_color;
    }
    else{
        active_color = base_color;
    }
};

void RectWidget::draw(std::vector<Element>& buffer){
    buffer.emplace_back(std::in_place_type<RectElement>,pos,size,active_color);
};

void RectWidget::setPosition(const Vec2& new_pos){
    pos = new_pos;
}

Vec2 RectWidget::getPosition(){
    return pos;
}

void RectWidget::setSize(const Vec2& new_size){
    size = new_size;
}

Vec2 RectWidget::getSize(){
    return size;
}

#pragma endregion

#pragma region LabelWidget
LabelWidget::LabelWidget(const LabelParam& data,TextMeasurer& Ameasurer){
    text = data.text;
    pos = data.pos;
    text_size = data.text_size;
    base_color = data.base_color;
    hover_color = data.hover_color;
    active_color = base_color;
    font_id = data.font_id;
    type = WidgetType::Label;
    measurer = Ameasurer;
    size = measurer(text,font_id,text_size).size;
}

void LabelWidget::update(Mouse& mouse){
    if(rectPointCollision(pos,size,mouse.mouse_pos)){
        active_color = hover_color;
    }
    else{
        active_color = base_color;
    }
}

void LabelWidget::draw(std::vector<Element>& buffer){
    buffer.emplace_back(std::in_place_type<TextElement>,text,text_size,pos,active_color);
}

void LabelWidget::setPosition(const Vec2& new_pos){
    pos = new_pos;
}

Vec2 LabelWidget::getPosition(){
    return pos;
}

void LabelWidget::setSize(const Vec2& new_size){
    size = new_size;
}

Vec2 LabelWidget::getSize(){
    return size;
}

#pragma endregion

#pragma region ButtonWidget
ButtonWidget::ButtonWidget(const ButtonParam& data){
    type = WidgetType::Button;
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
            // std::cout << "Button clicked: " << text << std::endl;
            active_color = bg_color;
        }
    }
    else{
        active_color = bg_color;
    }
}

void ButtonWidget::draw(std::vector<Element>& buffer){
    buffer.emplace_back(std::in_place_type<RectElement>,pos,size,active_color);
    buffer.emplace_back(std::in_place_type<TextElement>,text,text_size,(pos - pos_offset) + Vec2{padding/2,padding/2},text_color);
}

void ButtonWidget::setPosition(const Vec2& new_pos){
    pos = new_pos;
}

Vec2 ButtonWidget::getPosition(){
    return pos;
}

void ButtonWidget::setSize(const Vec2& new_size){
    size = new_size;
}

Vec2 ButtonWidget::getSize(){
    return size;
}

#pragma endregion

#pragma region SliderWidget
SliderWidget::SliderWidget(const SliderParam& data){
    type = WidgetType::Slider;
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
        handle_pos = Vec2{track_pos.x + (value-range.x)/(range.y-range.x)*(track_size.x-handle_size.x),track_pos.y - (handle_size.y - track_size.y)/2};
    }
    else{
        handle_pos = Vec2{track_pos.x - (handle_size.x - track_size.x)/2,track_pos.y + (value-range.x)/(range.y-range.x)*(track_size.y-handle_size.y)};
    }
}

bool SliderWidget::isHovering(const Vec2& mouse_pos){
    return rectPointCollision(handle_pos, handle_size, mouse_pos);
}

void SliderWidget::update(Mouse& mouse){
    if(isHovering(mouse.mouse_pos)){
        handle_active_color = handle_hover_color;
        if(!(mouse.buttons[MouseButtonType::Left].wasPressed) & mouse.buttons[MouseButtonType::Left].isPressed){
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

void SliderWidget::setPosition(const Vec2& new_pos){
    track_pos = new_pos;
    if(orientation){
        handle_pos = Vec2{track_pos.x + (value-range.x)/(range.y-range.x)*(track_size.x-handle_size.x),track_pos.y - (handle_size.y - track_size.y)/2};
    }
    else{
        handle_pos = Vec2{track_pos.x - (handle_size.x - track_size.x)/2,track_pos.y + (value-range.x)/(range.y-range.x)*(track_size.y-handle_size.y)};
    }
}

Vec2 SliderWidget::getPosition(){
    return track_pos;
}

void SliderWidget::setSize(const Vec2& new_size){
    track_size = new_size;
    if(orientation){
        handle_pos = Vec2{track_pos.x + (value-range.x)/(range.y-range.x)*(track_size.x-handle_size.x),track_pos.y - (handle_size.y - track_size.y)/2};
    }
    else{
        handle_pos = Vec2{track_pos.x - (handle_size.x - track_size.x)/2,track_pos.y + (value-range.x)/(range.y-range.x)*(track_size.y-handle_size.y)};
    }
}

Vec2 SliderWidget::getSize(){
    return track_size;
}

#pragma endregion

#pragma region Group

GroupWidget::GroupWidget(const GroupParam& data): bg(RectParam{data.pos,data.size,data.bg_color,data.hover_color}){
}

void GroupWidget::update(Mouse& mouse){
    for(auto& widget:widgets){
        widget->update(mouse);
    }
}

void GroupWidget::draw(std::vector<Element>& buffer){
    bg.draw(buffer);
    for(auto& widget: widgets){
        widget->draw(buffer);
    }
}

void GroupWidget::setPosition(const Vec2& new_pos){
    bg.setPosition(new_pos);
}

Vec2 GroupWidget::getPosition(){
    return bg.getPosition();
}

void GroupWidget::setSize(const Vec2& new_size){
    bg.setSize(new_size);
}

Vec2 GroupWidget::getSize(){
    return bg.getSize();
}

#pragma endregion

#pragma region Mouse
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
    if(buttons[type].isPressed && buttons[type].wasPressed & (std::abs(drag.x) >= drag_threshold || std::abs(drag.y) >= drag_threshold)){
        return true;
    }
    return false;
}

#pragma endregion

UI::UI(json& data,TextMeasurer m){
    measure = m;
    addGroup(data["ui"],data["defaults"],widgets);
}

void UI::addGroup(json& data,json& defaults,std::vector<std::unique_ptr<Widget>>& parent_widgets){
    auto getDataWithFallback = [&](const std::string& key) {
    return data.contains(key) ? data.at(key) : defaults.at(key);
    };
    GroupWidget group(GroupParam{getVec2(data.at("pos")),Vec2{0,0},getColor(getDataWithFallback("bg_color")),getColor(getDataWithFallback("hover_color"))});
    Vec2 size = Vec2{data.at("padding"),data.at("padding")};
    if (getDataWithFallback("positioning") == "automatic"){
        Vec2 element_pos = Vec2{data.at("pos")[0],data.at("pos")[1]} + Vec2{data.at("padding"),data.at("padding")};
        for(auto& element:data.at("widgets")){
            if(element.at("type") == "Button"){
                group.widgets.emplace_back(std::make_unique<ButtonWidget>(parseButtonData(element,defaults)));
                group.widgets.back()->setPosition(element_pos);
            }
            else if(element.at("type") == "Slider"){
                group.widgets.emplace_back(std::make_unique<SliderWidget>(parseSliderData(element,defaults)));
                group.widgets.back()->setPosition(element_pos);
            }
            else if(element.at("type") == "Rect"){
                group.widgets.emplace_back(std::make_unique<RectWidget>(parseRectData(element,defaults)));
                group.widgets.back()->setPosition(element_pos);
            }
            else if(element.at("type") == "Label"){
                group.widgets.emplace_back(std::make_unique<LabelWidget>(parseLabelData(element,defaults),measure));
                group.widgets.back()->setPosition(element_pos);
            }
            else if (element.at("type") == "Group"){
                auto temp_data = element;
                temp_data["pos"]  = {element_pos.x,element_pos.y};
                addGroup(temp_data,defaults,group.widgets);
            }

            Vec2 element_size = group.widgets.back()->getSize();
            if(data.at("orientation") == "horizontal"){
                element_pos.x += element_size.x + (float)data.at("padding");
                size.x += element_size.x + (float)data.at("padding");
                size.y = std::max(size.y,element_size.y);
            }
            else{
                element_pos.y += element_size.y + (float)data.at("padding");
                size.y += element_size.y + (float)data.at("padding");
                size.x = std::max(size.x,element_size.x);
            }
        }

        if(data.at("orientation") == "horizontal"){
            size.y += 2*(float)data.at("padding");
        }
        else{
            size.x += 2*(float)data.at("padding");
        }
    }
    else{
        for(auto& element:data.at("widgets")){
            if(element.at("type") == "Button"){
                group.widgets.emplace_back(std::make_unique<ButtonWidget>(parseButtonData(element,defaults)));
                group.widgets.back()->setPosition(group.widgets.back()->getPosition() + getVec2(data["pos"]));
            }
            else if(element.at("type") == "Slider"){
                group.widgets.emplace_back(std::make_unique<SliderWidget>(parseSliderData(element,defaults)));
                group.widgets.back()->setPosition(group.widgets.back()->getPosition() + getVec2(data["pos"]));
            }
            else if(element.at("type") == "Rect"){
                group.widgets.emplace_back(std::make_unique<RectWidget>(parseRectData(element,defaults)));
                group.widgets.back()->setPosition(group.widgets.back()->getPosition() + getVec2(data["pos"]));
            }
            else if(element.at("type") == "Label"){
                group.widgets.emplace_back(std::make_unique<LabelWidget>(parseLabelData(element,defaults),measure));
                group.widgets.back()->setPosition(group.widgets.back()->getPosition() + getVec2(data["pos"]));
            }
            else if (element.at("type") == "Group"){
                auto temp_data = element;
                temp_data["pos"] = {(float)(element.at("pos")[0]) + (float)(data.at("pos")[0]),(float)(element.at("pos")[1]) + (float)(data.at("pos")[1])};
                addGroup(temp_data,defaults,group.widgets);
            }

            Vec2 element_size = group.widgets.back()->getSize();

            Vec2 widget_pos = group.widgets.back()->getPosition();

            float right = widget_pos.x + element_size.x;
            float bottom = widget_pos.y + element_size.y;
            Vec2 group_pos = getVec2(data["pos"]);

            size.x = std::max(size.x,right - group_pos.x);
            size.y = std::max(size.y,bottom - group_pos.y);
        }
        size = size +  Vec2{data.at("padding"),data.at("padding")};
    }

    Color bg_color = getColor(getDataWithFallback("bg_color"));
    Color hover_color = getColor(getDataWithFallback("hover_color"));
    group.setSize(size);
    parent_widgets.emplace_back(std::make_unique<GroupWidget>(std::move(group)));
}

void UI::update(){
    frame_render_buffer.clear();
    for(auto& widget: widgets){
        widget->update(mouse);
        widget->draw(frame_render_buffer);
    }
}

RectParam UI::parseRectData(json& data,json& defaults){
    auto getDataWithFallback = [&](const std::string& key) {
    return data.contains(key) ? data.at(key) : defaults.at(key);
    };

    RectParam param;
    param.pos = getVec2(data.at("pos"));
    param.size = getVec2(data.at("size"));
    param.base_color = getColor(getDataWithFallback("bg_color"));
    param.hover_color = getColor(getDataWithFallback("hover_color"));

    return param;
}

LabelParam UI::parseLabelData(json& data, json& defaults){
    auto getDataWithFallback = [&](const std::string& key) {
    return data.contains(key) ? data.at(key) : defaults.at(key);
    };

    LabelParam param;
    param.text = data.at("text");
    param.pos = getVec2(data.at("pos"));
    param.font_id = getDataWithFallback("font_id");
    param.text_size = getDataWithFallback("text_size");
    param.base_color = getColor(getDataWithFallback("bg_color"));
    param.hover_color = getColor(getDataWithFallback("hover_color"));

    return param;
}

ButtonParam UI::parseButtonData(json& data,json& defaults){
    auto getDataWithFallback = [&](const std::string& key) {
    return data.contains(key) ? data.at(key) : defaults.at(key);
    };

    ButtonParam param;
    param.text_color = getColor(getDataWithFallback("text_color"));
    param.text_size = getDataWithFallback("text_size");
    param.font_id = getDataWithFallback("font_id");
    param.text = data.at("text");
    BoundingBox bb = measure(param.text,param.font_id,param.text_size);
    param.pos = getVec2(data.at("pos"));
    param.padding = getDataWithFallback("padding");
    param.size = Vec2{bb.size.x + param.padding,bb.size.y + param.padding};
    param.pos_offset = bb.pos;
    param.bg_color = getColor(getDataWithFallback("bg_color"));
    param.hover_color = getColor(getDataWithFallback("hover_color"));

    return param;
};

SliderParam UI::parseSliderData(json& data,json& defaults){
    auto getDataWithFallback = [&](const std::string& key) {
    return data.contains(key) ? data.at(key) : defaults.at(key);
    };

    SliderParam param;
    param.track_pos = getVec2(data.at("pos"));

    if(data.at("orientation") == "horizontal"){
        param.orientation = true;
    }
    else{
        param.orientation = false;
    }

    param.track_size = getVec2(getDataWithFallback("track_size"));
    param.handle_size = getVec2(getDataWithFallback("handle_size"));
    param.range = getVec2(getDataWithFallback("range"));
    param.value = getDataWithFallback("value");
    param.step = getDataWithFallback("step");
    param.track_color = getColor(getDataWithFallback("track_color"));
    param.handle_base_color = getColor(getDataWithFallback("handle_base_color"));
    param.handle_hover_color = getColor(getDataWithFallback("handle_hover_color"));

    return param;
}