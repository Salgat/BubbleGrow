//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"

Renderer::Renderer()
    : mouse_movement(true)
    , current_menu(MenuType::MAIN)
    , mode(GameMode::IN_GAME)
    , show_ingame_menu(false) {
    std::string title = "BubbleGrow V" + std::to_string(VERSION) + "." + std::to_string(SUB_VERSION);
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), title);
    //window->setFramerateLimit(60);

    if (!font.loadFromFile("data/fonts/Sile.ttf")) {

    } else {
        text.setFont(font);
        text.setCharacterSize(24);
    }

    // Calculate pixel heights of each font size (is there a way to better automate this?)
    text.setString("Testing");

    text.setCharacterSize(TextSize::GAME_MENU);
    text_heights[TextSize::GAME_MENU] = text.getLocalBounds().height;
    text.setCharacterSize(TextSize::RESOURCE_COUNTER);
    text_heights[TextSize::RESOURCE_COUNTER] = text.getLocalBounds().height;
}

/**
 * Handles pending events created by the input. Returns false to end game loop.
 */
bool Renderer::PollEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            return false;
        } else if (mode == GameMode::IN_GAME) {
            if(!GamePollEvents(event))
                return false;
        } else if (mode == GameMode::MENU) {
            if(!MenuPollEvents(event))
                return false;
        }
    }

    return true;
}

/**
 * Handles pending events for in-game.
 */
bool Renderer::GamePollEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            // Toggle movement based on mouse cursor
            mouse_movement = !mouse_movement;

            // Reset move request so player stops moving
            player->PlayerMoveRequest(player->position, 0.0);
        } else if (event.key.code == sf::Keyboard::Escape) {
            show_ingame_menu = !show_ingame_menu;
            if (show_ingame_menu) {
                mouse_movement = false;
            } else {
                mouse_movement = true;
            }
        }
    }

    return true;
}

/**
 * Handles pending events for menu.
 */
bool Renderer::MenuPollEvents(sf::Event& event) {
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
    } else {
        player->PlayerMoveRequest(sf::Vector2f(player->position.x, player->position.y), 1.0);
    }
}

/**
 * Render the game.
 */
void Renderer::RenderGame() {
    window->clear(sf::Color(255, 255, 255));

    // Render based on if the player is using the menu (no active game) or playing a game
    if (mode == GameMode::IN_GAME) {
        RenderMap();
        RenderUnits();
        RenderInterface();
    } else if (mode == GameMode::MENU) {
        RenderMenu();
    }

    window->display();
}

/**
 * Render the land for the game.
 */
void Renderer::RenderMap() {
    // Idea: Have a tilemap randomly generated (for example, use 9 tiles of 1024x1024 size with a 2d array to
    //       tell what to display for background).

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
 * Render the interface for in-game.
 */
void Renderer::RenderInterface() {
    // Display resource count
    RenderText("Resources: " + std::to_string(player->resources), sf::Vector2f(10.0, 10.0), TextSize::RESOURCE_COUNTER);

    if (show_ingame_menu) {
        current_menu = MenuType::GAME_MENU;
        RenderMenuText(current_menu);
    }

}

/**
 * Renders the start menu (not the in-game menu).
 */
void Renderer::RenderMenu() {

}

/**
 * Renders text at specified location and attributes.
 */
void Renderer::RenderText(std::string display_text, sf::Vector2f location, unsigned int font_size, sf::Color color) {
    text.setString(display_text);
    text.setStyle(sf::Text::Regular);
    text.setColor(color);
    text.setPosition(location);
    text.setCharacterSize(font_size);
    text.setOrigin(0.0, 0.0);
    window->draw(text);
}

/**
 * Renders the text for the current menu selection.
 */
void Renderer::RenderMenuText(MenuType selection) {
    // Setup a list of all entries to be rendered for current menu.
    std::vector<MenuType> menu_entries;
    float text_height;
    if (selection == MenuType::GAME_MENU) {
        for (auto entry : kGameSubMenus) {
            menu_entries.push_back(entry);
        }

        text.setCharacterSize(TextSize::GAME_MENU);
        text_height = text_heights[TextSize::GAME_MENU];
    }

    auto menu_height = menu_entries.size() * text_height;
    auto y_offset = RESOLUTION_Y/2 - menu_height/2;

    // Draw each menu entry
    for (auto entry : menu_entries) {
        text.setString(kMenuStrings[static_cast<std::size_t>(entry)]);
        text.setOrigin(text.getLocalBounds().width/2.0, text.getLocalBounds().height/2.0);
        text.setPosition(sf::Vector2f(RESOLUTION_X/2, y_offset));
        window->draw(text);
        y_offset += text_height;
    }
}

