//
// Created by Austin on 5/24/2015.
//

#ifndef BUBBLEGROW_EVENT_HPP
#define BUBBLEGROW_EVENT_HPP

#include "common.hpp"


enum class EventType {
    // Gameplay events
    BUBBLE_DIE,
    BUBBLE_ATTACK,
    BUBBLE_GATHER,
    WON_GAME,
    LOST_GAME,

    // Menu Events
    MENU_SELECTION,
    MENU_SELECTION_CHANGE,

    // General Events
    ENTER_GAME,
    ENTER_MAIN_MENU,

    NONE
};

/**
 * Holds data regarding events occuring in the game (such as unit dying or selecting a menu option).
 */
struct Event {
    EventType type;
    sf::Vector2f position;

    Event(EventType event_type, sf::Vector2f event_game_position = sf::Vector2f(0.0, 0.0))
       : type(event_type)
       , position(event_game_position) {}
};

#endif //BUBBLEGROW_EVENT_HPP
