#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "UI.hpp"

/*
TODO: add a way to let the button have custom sizes instead of just the size of the text + padding (if the size is smaller
 than the text + padding then default to text + padding, if the size is bigger than text + padding then use the custom size)

 if the button have a custom size then the text should be centred in the button (toggle)

 add a way to let the group either set size to its children or have a custom size (if the size is smaller than the children then default to children size, 
 if the size is bigger than the children then use the custom size)

 if the group have a custom size then the children should be resized to fill the group size (toggle)

 let the slider show the value on top of it whenver it's handle is selected (toggle)
*/

using json = nlohmann::json;

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

int main() {
    #pragma region Initialising window
    sf::RenderWindow window(sf::VideoMode({800, 800}), "SFML 3 Test");
    #pragma endregion

    // Clock to track the delta time and dt_update_clock to update the rendered dt after some time instead of every frame
    sf::Clock clock;
    sf::Clock dt_update_clock;

    std::array<sf::Font,1> fonts;
    if(!(fonts[0].openFromFile("assets/PoetsenOne-Regular.ttf"))){
        std::cout << "Could not load font!" << std::endl;
    }

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
        }

        sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
        ui.mouse.update({mouse_pos.x,mouse_pos.y});
        ui.mouse.updateLeftMouseButton((sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)));

        ui.update();

        window.clear();

        for(auto& element: ui.frame_render_buffer){
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, RectElement>) {
                    drawRect(window,&arg);
                } else if constexpr (std::is_same_v<T, TextElement>) {
                    drawText(window,&arg,fonts[0]);
                }
            }, element);
        }

        window.draw(ms_text);

        window.display();
    }
}