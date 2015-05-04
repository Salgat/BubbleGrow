//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"

Renderer::Renderer() {
    std::string title = "BubbleGrow V" + std::to_string(VERSION) + "." + std::to_string(SUB_VERSION);
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), title);
}

/**
 * Handles pending events created by the input.
 */
bool Renderer::PollEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {

    }
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




    RenderInterface();

    window->display();
}

/**
 * Render the interface for the game.
 */
void Renderer::RenderInterface() {

}

