//
// Created by Austin on 5/26/2015.
//

#ifndef BUBBLEGROW_INPUTHANDLER_HPP
#define BUBBLEGROW_INPUTHANDLER_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Event.hpp"

class World;
class Player;
class Renderer;

/**
 * Processes all inputs from the player. This class exists as an extension of the Renderer (it is tightly coupled to
 * the renderer).
 */
class InputHandler {
private:

public:
    std::shared_ptr<Renderer> renderer;

    InputHandler(std::shared_ptr<Renderer> renderer_);

    bool PollEvents();
    void ProcessInputs(); // Use player->requests to add input requests

    bool GamePollEvents(sf::Event& event);
    bool MenuPollEvents(sf::Event& event);
};

#endif //BUBBLEGROW_INPUTHANDLER_HPP
