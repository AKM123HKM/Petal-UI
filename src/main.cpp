#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "UI.hpp"

using json = nlohmann::json;

// TEMPORARY
SizingMode getMode(const json& data){
    std::string mode = data;
    if (mode == "fill"){
        return SizingMode::Fill;
    }
    else if (mode == "fixed"){
        return SizingMode::Fixed;
    }
    
    return SizingMode::Fit;
}

void drawRect(sf::RenderWindow& window, RectElement* element){
    sf::RectangleShape rect({element->size.x,element->size.y});
    rect.setPosition({element->pos.x,element->pos.y});
    rect.setFillColor({element->color.r,element->color.g,element->color.b});
    window.draw(rect);
}

void drawText(sf::RenderWindow& window, TextElement* element, sf::Font& font){
    sf::Text text(font,element->text,(unsigned)(element->size));
    text.setFillColor({element->color.r,element->color.g,element->color.b});
    text.setPosition({element->pos.x,element->pos.y});
    window.draw(text);
}

// TEMPORARY
UIElement parseElementData(json& data,UIElement* parent){
    UIElement element;
    if(data.contains("direction")){
    element.direction = data.at("direction") == "column" ? OrderDirection::Col : OrderDirection::Row;}
    else{
        element.direction = OrderDirection::Row;
    }
    element.bg_color = Color{data.at("background_color")[0],data.at("background_color")[1],data.at("background_color")[2]};
    element.padding = Padding{data.at("padding")[0],data.at("padding")[1],data.at("padding")[2],data.at("padding")[3]};
    element.child_gap = data.contains("child_gap") ? (float)data.at("child_gap") : 0.0f;
    auto width_data = data.at("width");
    auto height_data = data.at("height");
    element.width = Edge{
        width_data.at("value"),
        getMode(width_data.at("mode")),
        width_data.at("grow"),
        width_data.at("shrink"),
        width_data.contains("min") ? (float)width_data.at("min") : 0,
        width_data.contains("max") ? (float)width_data.at("max") : INFINITY
    };
    element.height = Edge{
        height_data.at("value"),
        getMode(height_data.at("mode")),
        height_data.at("grow"),
        height_data.at("shrink"),
        height_data.contains("min") ? (float)height_data.at("min") : 0,
        height_data.contains("max") ? (float)height_data.at("max") : INFINITY
    };
    element.parent = parent;

    if(data.contains("children")){
        for(auto& child_data: data.at("children")){
            UIElement child_element = parseElementData(child_data,&element);
            element.children.emplace_back(std::make_unique<UIElement>(std::move(child_element)));
        }
    }

    return element;
}

// TEMPORARY
void printTree(UIElement* root){
    std::cout << "pos: " << root->pos.x << ", " << root->pos.y << std::endl;
    std::cout << "size: " << root->width.value << ", " << root->height.value << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    for (auto& child: root->children){
        printTree(child.get());
    }
}

// TEMPORARY
void drawTree(sf::RenderWindow& window, UIElement* element){
    sf::RectangleShape rect(sf::Vector2f(element->width.value,element->height.value));
    rect.setPosition(sf::Vector2f(element->pos.x, element->pos.y));
    rect.setFillColor(sf::Color(element->bg_color.r, element->bg_color.g, element->bg_color.b));

    window.draw(rect);

    for (auto& child: element->children){
        drawTree(window,child.get());
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800,800}), "SFML 3 Test");

    // Clock to track the delta time and dt_update_clock to update the rendered dt after some time instead of every frame
    sf::Clock clock;
    sf::Clock dt_update_clock;

    std::array<sf::Font,1> fonts;
    if(!(fonts[0].openFromFile("assets/PoetsenOne-Regular.ttf"))){
        std::cout << "Could not load font!" << std::endl;
    }

    std::ifstream o("assets/data2.json");
    json data2 = json::parse(o);
    UIElement root = parseElementData(data2.at("root"),nullptr);

    std::ifstream f("assets/data.json");
    json data = json::parse(f);

    UI ui(data,
        [&](const std::string& text,int font_id,float size){
        sf::Text temp_text(fonts[font_id],text,(unsigned)size);
        sf::FloatRect bounds = temp_text.getLocalBounds();
        return BoundingBox{Vec2{bounds.position.x,bounds.position.y},Vec2{bounds.size.x,bounds.size.y}};
    });

    float dt = 0.0f;
    // ms_text = microsecond text
    sf::Text ms_text(fonts[0],std::to_string(dt),32);
    ms_text.setPosition(sf::Vector2f(0,0));
    ms_text.setFillColor(sf::Color(255,255,255));

    sf::Vector2f window_size;

    while (window.isOpen()) {
        dt = clock.getElapsedTime().asMicroseconds();
        clock.restart();

        if(dt_update_clock.getElapsedTime().asSeconds() >= 1){
            ms_text.setString(std::to_string(dt));
            dt_update_clock.restart();
        }
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea({0.f,0.f},{(float)resized->size.x, (float)resized->size.y});
                window.setView(sf::View(visibleArea));
                root.width.value = (float)resized->size.x;
                root.height.value = (float)resized->size.y;
            }
        }

        sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
        ui.mouse.update({mouse_pos.x,mouse_pos.y});
        ui.mouse.updateLeftMouseButton((sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)));

        ui.update();
        computeLayout(&root);        

        window.clear();

        // for(auto& element: ui.frame_render_buffer){
        //     std::visit([&](auto&& arg) {
        //         using T = std::decay_t<decltype(arg)>;
        //         if constexpr (std::is_same_v<T, RectElement>) {
        //             drawRect(window,&arg);
        //         } else if constexpr (std::is_same_v<T, TextElement>) {
        //             drawText(window,&arg,fonts[0]);
        //         }
        //     }, element);
        // }
        drawTree(window,&root);
        window.draw(ms_text);

        window.display();
    }
}