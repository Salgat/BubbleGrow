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
        } else if (event.type == sf::Event::KeyPressed) {
            // Todo: This is just a test, move this to ProcessInputs() and instead use the distance of the cursor
            //       from the center to tell it which direction to go. Possibly can add a third float_data which
            //       gives a value between 0.0 and 1.0 to scale speed.
            Request move_request;
            move_request.type = RequestType::PLAYER_WALK;
            move_request.float_data[0] = player->position.x;
            move_request.float_data[1] = player->position.y;

            if (event.key.code == sf::Keyboard::Up) {
                move_request.float_data[1] -= 2.0;
            } else if (event.key.code == sf::Keyboard::Down) {
                move_request.float_data[1] += 2.0;
            } else if (event.key.code == sf::Keyboard::Left) {
                move_request.float_data[0] -= 2.0;
            } else if (event.key.code == sf::Keyboard::Right) {
                move_request.float_data[0] += 2.0;
            }

            player->requests_array[static_cast<std::size_t>(RequestType::PLAYER_WALK)] = move_request;
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

