#pragma once
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include "json.hpp"
#include <iostream>
#include <variant>

using json = nlohmann::json;

struct Vec2{
    float x,y;
};

float magnitude(const Vec2& v1,const Vec2& v2);

struct Color{
    std::uint8_t r,g,b;
};

struct BoundingBox{
    Vec2 pos, size;
};

using TextMeasurer = std::function<BoundingBox(const std::string& text, int font_id, float size)>;

struct RectElement{
    Vec2 pos;
    Vec2 size;
    Color color;

    RectElement() = default;
    RectElement(const Vec2& Apos,const Vec2& Asize,const Color& Acolor);
};

struct TextElement{
    std::string text;
    Vec2 pos;
    Color color;
    float size;

    TextElement() = default;
    TextElement(const std::string& Atext,float Asize,const Vec2& Apos,const Color& Acolor);
};

using Element = std::variant<RectElement,TextElement>;

enum WidgetType{
    Button
};

struct Widget{
    WidgetType type;
    virtual void update(Vec2 mouse_pos,bool click) = 0;
    virtual void draw(std::vector<Element>& buffer) = 0;
};

struct ButtonParam{
    WidgetType type;
    std::string text;
    Vec2 pos;
    Vec2 pos_offset;
    Vec2 size;
    Color bg_color;
    Color hover_color;
    Color text_color;
    int font_id;
    float text_size;
    float padding;
};

struct ButtonWidget:Widget{
    Vec2 pos;
    Vec2 pos_offset; // offset required to align the text and the rectangle (don't ask why)
    Vec2 size;
    Color active_color;
    Color bg_color;
    Color hover_color;
    std::string text;
    Color text_color;
    int font_id;
    float text_size;
    float padding;

    ButtonWidget(const ButtonParam& data);
    
    bool isHovering(const Vec2& mouse_pos);
    void update(Vec2 mouse_pos, bool click);
    void draw(std::vector<Element>& buffer);
};

struct MouseButton{
    bool isPressed;
    bool wasPressed;
};

enum MouseButtonType{
    Left,
    Middle,
    Right
};

struct Mouse{
    Vec2 mouse_pos = {INFINITY,INFINITY};
    std::unordered_map<MouseButtonType,MouseButton> buttons;
    Vec2 drag;
    float drag_threshold = 3;

    Mouse();
    void update(const std::array<float,2>& pos);
    void updateLeftMouseButton(bool AisPressed);
    void updateRightMouseButton(bool AisPressed);
    void updateMiddleMouseButton(bool AisPressed);
    bool isClicked(MouseButtonType type);
};

class UI{
public:
    std::vector<std::unique_ptr<Widget>> widgets;
    std::vector<Element> frameRenderBuffer;
    Mouse mouse;

    UI(json& data,TextMeasurer m);
    void update();

private:
    TextMeasurer measure;
    ButtonParam parseButtonData(json& data);
    void addButton(ButtonParam& data);
};