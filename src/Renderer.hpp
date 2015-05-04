//
// Created by Austin on 5/3/2015.
//

#ifndef BUBBLEGROW_RENDERER_HPP
#define BUBBLEGROW_RENDERER_HPP

#include "common.hpp"
#include "Request.hpp"

class World;
class Player;

class Renderer {
private:


public:
    std::shared_ptr<sf::RenderWindow> window;

    std::shared_ptr<World> world;
    std::shared_ptr<Player> player;

    Renderer();

    bool PollEvents();
    void ProcessInputs(); // Use player->requests to add input requests

    void RenderWorld();

    void RenderLand();
    void RenderUnits();
    void RenderInterface();
};

#endif //BUBBLEGROW_RENDERER_HPP
