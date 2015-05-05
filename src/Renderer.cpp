//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"

Renderer::Renderer() {
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
        }
    }

    return true;
}

/**
 * Handles non-event input handling.
 */
void Renderer::ProcessInputs() {

}

/**
 * Render the current game world.
 */
void Renderer::RenderWorld() {
    window->clear();

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
    sf::CircleShape circle(5);
    circle.setFillColor(sf::Color(100, 250, 50));

    for (auto& player_reference : world->players) {
        for (auto& unit : player_reference.second->units) {
            circle.setPosition((unit.second->position.x - player_position.x)*scale + RESOLUTION_X/2,
                               (unit.second->position.y - player_position.y)*scale + RESOLUTION_Y/2);
            window->draw(circle);
        }
    }
}

/**
 * Render the interface for the game.
 */
void Renderer::RenderInterface() {

}

