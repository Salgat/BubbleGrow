//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"

Renderer::Renderer()
    : mouse_movement(true) {
    std::string title = "BubbleGrow V" + std::to_string(VERSION) + "." + std::to_string(SUB_VERSION);
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), title);
    //window->setFramerateLimit(60);
}

/**
 * Handles pending events created by the input.
 */
bool Renderer::PollEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            return false;
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                // Toggle movement based on mouse cursor
                mouse_movement = !mouse_movement;

                // Reset move request so player stops moving
                player->PlayerMoveRequest(player->position, 0.0);
            }
        }
    }

    return true;
}

/**
 * Handles non-event input handling.
 */
void Renderer::ProcessInputs() {
    if (mouse_movement) {
        // Movement based on cursor's position from center of screen.
        auto mouse_position = sf::Mouse::getPosition(*window);
        auto distance_x = static_cast<int>(mouse_position.x) - static_cast<int>(RESOLUTION_X/2);
        auto distance_y = static_cast<int>(mouse_position.y) - static_cast<int>(RESOLUTION_Y/2);
        float distance_from_center = std::sqrt(distance_x*distance_x + distance_y*distance_y);

        // Speed is based on the cursor's distance from the center (with a small area in the center where there is no
        // speed).
        float speed = (distance_from_center-50.0)/200.0;
        if (speed > 1.0)
            speed = 1.0;
        else if (speed < 0.0)
            speed = 0.0;

        player->PlayerMoveRequest(sf::Vector2f(player->position.x + distance_x, player->position.y + distance_y), speed);
    }
}

/**
 * Render the current game world.
 */
void Renderer::RenderWorld() {
    window->clear(sf::Color(255, 255, 255));

    RenderMap();
    RenderUnits();
    RenderInterface();

    window->display();
}

/**
 * Render the land for the game.
 */
void Renderer::RenderMap() {

}

/**
 * Render the units for the game.
 */
void Renderer::RenderUnits() {
    if (!player or !world)
        return;

    auto player_position = player->position;
    double scale = 20.0;

    // For now just drawing tiny circles until we can get real graphics done
    sf::Color player_colors[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow};
    std::size_t count = 0;
    sf::CircleShape circle(10);
    for (auto& player_reference : world->players) {
        for (auto& unit : player_reference.second->units) {
            if (unit.second->health[0] > 0) {
                circle.setFillColor(player_colors[count % sizeof(player_colors)]);
            } else {
                circle.setFillColor(sf::Color::Black);
            }

            auto radius = unit.second->size*10.0;
            circle.setRadius(radius);
            circle.setOrigin(circle.getLocalBounds().width/2.0, circle.getLocalBounds().height/2.0);
            circle.setPosition((unit.second->position.x - player_position.x)*scale + RESOLUTION_X/2,
                               (unit.second->position.y - player_position.y)*scale + RESOLUTION_Y/2);

            window->draw(circle);
        }
        count++;
    }
}

/**
 * Render the interface for the game.
 */
void Renderer::RenderInterface() {

}

