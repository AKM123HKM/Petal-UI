#pragma once
#include <array>
#include "json.hpp"
#include <iostream>
#include <variant>
#include "layout.hpp"


using json = nlohmann::json;

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
    float drag_threshold = 1;

    Mouse();
    void update(const std::array<float,2>& pos);
    void updateLeftMouseButton(bool AisPressed);
    void updateRightMouseButton(bool AisPressed);
    void updateMiddleMouseButton(bool AisPressed);
    bool isDragging(MouseButtonType type);
    bool isClicked(MouseButtonType type);
};

enum WidgetType{
    Rectangle,
    Label,
    Button,
    Slider,
    Group
};

class Widget{
    public:
        WidgetType type;
        Widget* parent;
        Vec2 pos;
        Vec2 size;

        virtual ~Widget() = default;
        virtual void update(Mouse& mouse) = 0;
        virtual void draw(std::vector<Element>& buffer) = 0;
        virtual void setPosition(const Vec2& new_pos);
        virtual Vec2 getPosition();
        virtual void setSize(const Vec2& new_size);
        virtual Vec2 getSize();
};

struct RectParam{
    Vec2 pos;
    Vec2 size;
    Color base_color;
    Color hover_color;
};

class RectWidget:public Widget{
    Color base_color;
    Color hover_color;
    Color active_color;

    public:
        RectWidget(const RectParam& data);
        void update(Mouse& mouse);
        void draw(std::vector<Element>& buffer);
};

struct LabelParam{
    std::string text;
    Vec2 pos;
    Vec2 size;
    Color base_color;
    Color hover_color;
    float text_size;
    int font_id;
};

class LabelWidget:public Widget{
    std::string text;
    Color base_color;
    Color hover_color;
    Color active_color;
    float text_size;
    int font_id;
    TextMeasurer measurer;

    public:
        LabelWidget(const LabelParam& data, TextMeasurer& Ameasurer);
        void update(Mouse& mouse);
        void draw(std::vector<Element>& buffer);
};

struct ButtonParam{
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

class ButtonWidget:public Widget{
    Vec2 pos_offset; // offset required to align the text and the rectangle (don't ask why)
    Color active_color;
    Color bg_color;
    Color hover_color;
    Color text_color;
    int font_id;
    float text_size;
    float padding;


    public:
        std::string text;
        ButtonWidget(const ButtonParam& data);
        bool isHovering(const Vec2& mouse_pos);
        void update(Mouse& mouse);
        void draw(std::vector<Element>& buffer);
};

struct SliderParam{
    Vec2 range;
    Vec2 track_pos;
    Vec2 track_size;
    Vec2 handle_size;
    Color track_color;
    Color handle_base_color;
    Color handle_hover_color;
    float value;
    float step;
    bool orientation;
};

class SliderWidget:public Widget{
    Vec2 range;
    Vec2 handle_pos;
    Vec2 handle_size;
    Color track_color;
    Color handle_base_color;
    Color handle_hover_color;
    Color handle_active_color;
    WidgetType type;
    float value;
    float step;
    bool orientation;
    bool selected;

    public:
        SliderWidget(const SliderParam& data);
        bool isHovering(const Vec2& mouse_pos);
        void update(Mouse& mouse);
        void draw(std::vector<Element>& buffer);
        void setPosition(const Vec2& new_pos) override;
        void setSize(const Vec2& new_size) override;
};

struct GroupParam{
    Vec2 pos;
    Vec2 size;
    Color bg_color;
    Color hover_color;
};

class GroupWidget:public Widget{
    Color active_color;
    Color bg_color;
    Color hover_color;
    public:
        std::vector<std::unique_ptr<Widget>> widgets;
        GroupWidget(const GroupParam& data);
        void update(Mouse& mouse);
        void draw(std::vector<Element>& buffer);
};

class UI{
public:
    std::vector<std::unique_ptr<Widget>> widgets;
    std::vector<Element> frame_render_buffer;
    Mouse mouse;

    UI(json& data,TextMeasurer m);
    void update();

private:
    TextMeasurer measure;
    RectParam parseRectData(json& data,json& defaults);
    LabelParam parseLabelData(json& data,json& defaults);
    ButtonParam parseButtonData(json& data,json& defaults);
    SliderParam parseSliderData(json& data,json& defaults);
    void addGroup(json& data,json& defaults,std::vector<std::unique_ptr<Widget>>& parent_widgets);
};