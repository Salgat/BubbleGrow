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
    , last_menu(MenuType::NONE)
    , mode(GameMode::MENU)
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

    text.setCharacterSize(TextSize::MAIN);
    text_heights[TextSize::MAIN] = text.getLocalBounds().height;
    text.setCharacterSize(TextSize::VERSUS);
    text_heights[TextSize::VERSUS] = text.getLocalBounds().height;

    // Load textures to be used by the game
    textures[ImageId::LOGO] = std::make_unique<sf::Texture>();
    textures[ImageId::LOGO]->loadFromFile("data/artwork/logo.png");
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
    static MenuType last_menu_item_clicked = MenuType::NONE;

    // Todo: Might want to encapsulate the mouse click check into a function?
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
        } else if (event.key.code == sf::Keyboard::Num1) {
            player->PlayerPurchaseRequest(10, UnitType::BASE);
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if mouse is clicking on any menu selection
            last_menu_item_clicked = MouseOverWhichMenuOption(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto menu_item_released_at = MouseOverWhichMenuOption(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            if (menu_item_released_at == last_menu_item_clicked and last_menu_item_clicked != MenuType::NONE) {
                // Menu item has been pressed (we checked if the mouse click and release is on the same menu item)
                if (menu_item_released_at == MenuType::LEAVE_GAME) {
                    // Leave the game and go back to the main menu (and do any cleanup)
                    world = nullptr;
                    player = nullptr;

                    show_ingame_menu = false;
                    mouse_movement = true;

                    current_menu = MenuType::MAIN;
                    mode = GameMode::MENU;
                } else if (menu_item_released_at == MenuType::BACK) {
                    // Close the menu
                    show_ingame_menu = false;
                    mouse_movement = true;
                }
            }
        }
    }

    return true;
}

/**
 * Handles pending events for menu.
 */
bool Renderer::MenuPollEvents(sf::Event& event) {
    // Check if any menu options are clicked
    static MenuType last_menu_item_clicked = MenuType::NONE;
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if mouse is clicking on any menu selection
            last_menu_item_clicked = MouseOverWhichMenuOption(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto menu_item_released_at = MouseOverWhichMenuOption(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            if (menu_item_released_at == last_menu_item_clicked and last_menu_item_clicked != MenuType::NONE) {
                // Menu item has been pressed (we checked if the mouse click and release is on the same menu item)
                if (menu_item_released_at == MenuType::QUICK_MATCH) {
                    // Quick Match clicked, setup a game for the player
                    // Todo: Set this up into a function with customizable parameters
                    world = std::make_shared<World>();
                    auto resource_player = world->AddResources(1000*10*10, 50*10, 500);

                    player = world->AddPlayer();
                    player->position = sf::Vector2f(0.0, 0.0);
                    player->name = "Test_Player";
                    player->resources = 1000;
                    player->CreateUnits(10, UnitType::BASE);

                    for (unsigned int count = 0; count < 4; ++count) {
                        auto enemy_player = world->AddPlayer();
                        enemy_player->position = enemy_player->RandomWanderLocation();
                        enemy_player->name = "Test_Computer_Player";
                        enemy_player->ai_type = AiType::EASY;
                        enemy_player->resources = 1000;
                        enemy_player->CreateUnits(10, UnitType::BASE);
                    }

                    current_menu = MenuType::GAME_MENU;
                    mode = GameMode::IN_GAME;
                } else if (menu_item_released_at == MenuType::EXIT_GAME) {
                    // Exit and close game
                    return false;
                } else if (menu_item_released_at == MenuType::VERSUS) {
                    current_menu = MenuType::VERSUS;
                    last_menu = MenuType::MAIN;
                } else if (menu_item_released_at == MenuType::BACK) {
                    current_menu = MenuType::MAIN;
                }
            }
        }
    }

    return true;
}

/**
 * Handles non-event input handling.
 */
void Renderer::ProcessInputs() {
    if (mode == GameMode::IN_GAME) {
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
    sf::Color player_colors[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow,
                                 sf::Color::Cyan, sf::Color::Magenta};
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
    // Render Logo
    sprite.setTexture(*textures[ImageId::LOGO]);
    sprite.setPosition(RESOLUTION_X/2 - sprite.getLocalBounds().width/2.0, 10.0);
    window->draw(sprite);

    // Render menu options
    RenderMenuText(current_menu);
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
    double y_offset = 0.0;
    TextSize text_size;
    if (selection == MenuType::GAME_MENU) {
        // Todo: Simplify this section (too repetitive)
        for (auto entry : kGameSubMenus) {
            menu_entries.push_back(entry);
        }

        text_size = TextSize::GAME_MENU;
        text.setCharacterSize(TextSize::GAME_MENU);
        text_height = text_heights[TextSize::GAME_MENU];
    } else if (selection == MenuType::MAIN) {
        for (auto entry : kMainSubMenus) {
            menu_entries.push_back(entry);
        }

        text_size = TextSize::MAIN;
        text.setCharacterSize(TextSize::MAIN);
        text_height = text_heights[TextSize::MAIN];

        // Offset y to make room for Logo
        y_offset += 100.0;
    } else if (selection == MenuType::VERSUS) {
        for (auto entry : kVersusSubMenus) {
            menu_entries.push_back(entry);
        }

        text_size = TextSize::MAIN;
        text.setCharacterSize(TextSize::MAIN);
        text_height = text_heights[TextSize::MAIN];

        // Offset y to make room for Logo
        y_offset += 100.0;
    }

    auto menu_height = menu_entries.size() * text_height;
    y_offset += RESOLUTION_Y/2 - menu_height/2;

    // Draw each menu entry
    menu_text_entries.clear();
    for (auto entry : menu_entries) {
        sf::Text menu_entry;
        menu_entry.setFont(font);
        menu_entry.setString(kMenuStrings[static_cast<std::size_t>(entry)]);
        menu_entry.setColor(sf::Color::Black);
        menu_entry.setCharacterSize(text_size);
        menu_entry.setOrigin(menu_entry.getLocalBounds().width/2.0, menu_entry.getLocalBounds().height/2.0);
        menu_entry.setPosition(sf::Vector2f(RESOLUTION_X/2, y_offset));
        menu_text_entries.push_back(std::pair<MenuType, sf::Text>(entry, menu_entry));

        window->draw(menu_entry);

        y_offset += text_height;
    }
}

/**
 * Returns true if the provided sf::Text object contains the location provided.
 */
bool Renderer::IsMouseOverText(sf::Text& text_object, sf::Vector2f cursor_location) {
    auto bounding_box = text_object.getGlobalBounds();
    if (bounding_box.contains(cursor_location))
        return true;
    return false;
}

/**
 * Returns the MenuType menu option that contains the location provided. Returns MenuType::NONE if none of the
 * menu options contain the location.
 */
MenuType Renderer::MouseOverWhichMenuOption(sf::Vector2f cursor_location) {
    // Iterate through each menu option to see if any contain the cursor location
    for (auto& entry : menu_text_entries) {
        if (IsMouseOverText(entry.second, sf::Vector2f(cursor_location.x, cursor_location.y))) {
            return entry.first;
        }
    }

    return MenuType::NONE;
}