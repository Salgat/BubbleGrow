//
// Created by Austin on 5/26/2015.
//

#include "InputHandler.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Renderer.hpp"
#include "Resources.hpp"
#include "SoundManager.hpp"

InputHandler::InputHandler(std::shared_ptr<Renderer> renderer_)
    : renderer(renderer_) {
}

/**
 * Handles pending events created by the input. Returns false to end game loop.
 */
bool InputHandler::PollEvents() {
    sf::Event event;
    while (renderer->window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            renderer->window->close();
            return false;
        } else if (event.type == sf::Event::Resized) {
            // Adjust resolution recognized by the game to the resized window resolution.
            ResolutionX = event.size.width;
            ResolutionY = event.size.height;

            // Scale by whichever dimension is changed the most
            renderer->UpdateView(sf::Vector2u(event.size.width, event.size.height));
        } else if (renderer->mode == GameMode::IN_GAME) {
            if(!GamePollEvents(event))
                return false;
        } else if (renderer->mode == GameMode::MENU) {
            if(!MenuPollEvents(event))
                return false;

            // Test if music or sound toggle clicked
            if (event.type == sf::Event::MouseButtonReleased) {
                auto mouse_position = renderer->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (renderer->sound_bounding_box.contains(mouse_position))
                    renderer->sound_on = !renderer->sound_on;
                if (renderer->music_bounding_box.contains(mouse_position))
                    renderer->music_on = !renderer->music_on;
            }
        }
    }

    return true;
}

/**
 * Handles non-event input handling.
 */
void InputHandler::ProcessInputs() {
    if (renderer->mode == GameMode::IN_GAME) {
        if (renderer->mouse_movement) {
            // Movement based on cursor's position from center of screen.
            auto mouse_position = renderer->window->mapPixelToCoords(sf::Mouse::getPosition(*(renderer->window)));

            auto distance_x = static_cast<int>(mouse_position.x) - static_cast<int>(ResolutionX /2);
            auto distance_y = static_cast<int>(mouse_position.y) - static_cast<int>(ResolutionY /2);
            float distance_from_center = std::sqrt(distance_x*distance_x + distance_y*distance_y);

            // Speed is based on the cursor's distance from the center (with a small area in the center where there is no
            // speed).
            float speed = (distance_from_center-50.0)/200.0;
            if (speed > 1.0)
                speed = 1.0;
            else if (speed < 0.0)
                speed = 0.0;

            renderer->player->PlayerMoveRequest(sf::Vector2f(renderer->player->position.x + distance_x, renderer->player->position.y + distance_y), speed);
        } else {
            renderer->player->PlayerMoveRequest(sf::Vector2f(renderer->player->position.x, renderer->player->position.y), 1.0);
        }
    }
}

/**
 * Handles pending events for in-game.
 */
bool InputHandler::GamePollEvents(sf::Event& event) {
    static MenuType last_menu_item_clicked = MenuType::NONE;

    // Todo: Might want to encapsulate the mouse click check into a function?
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            // Toggle movement based on mouse cursor
            renderer->mouse_movement = !(renderer->mouse_movement);

            // Reset move request so player stops moving
            renderer->player->PlayerMoveRequest(renderer->player->position, 0.0);
        } else if (event.key.code == sf::Keyboard::Escape) {
            renderer->show_ingame_menu = !(renderer->show_ingame_menu);
            if (renderer->show_ingame_menu) {
                renderer->mouse_movement = false;
            } else {
                renderer->mouse_movement = true;
            }
        } else if (event.key.code == sf::Keyboard::Num1) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::BASE_LV1)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::BASE_LV1);
        } else if (event.key.code == sf::Keyboard::Num2) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::BASE_LV2)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::BASE_LV2);
        } else if (event.key.code == sf::Keyboard::Num3) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::BASE_LV3)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::BASE_LV3);
        } else if (event.key.code == sf::Keyboard::Num4) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::GATHERER_LV1)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::GATHERER_LV1);
        } else if (event.key.code == sf::Keyboard::Num5) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::GATHERER_LV2)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::GATHERER_LV2);
        } else if (event.key.code == sf::Keyboard::Num6) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::GATHERER_LV3)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::GATHERER_LV3);
        } else if (event.key.code == sf::Keyboard::Num7) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::FIGHTER_LV1)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::FIGHTER_LV1);
        } else if (event.key.code == sf::Keyboard::Num8) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::FIGHTER_LV2)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::FIGHTER_LV2);
        } else if (event.key.code == sf::Keyboard::Num9) {
            if (renderer->player->resources >= kResourceCost[static_cast<std::size_t>(UnitType::FIGHTER_LV3)])
                renderer->player->PlayerPurchaseRequest(1, UnitType::FIGHTER_LV3);
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if mouse is clicking on any menu selection
            auto mouse_position = renderer->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            last_menu_item_clicked = renderer->MouseOverWhichMenuOption(mouse_position);
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto mouse_position = renderer->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            auto menu_item_released_at = renderer->MouseOverWhichMenuOption(mouse_position);
            if (menu_item_released_at == last_menu_item_clicked and last_menu_item_clicked != MenuType::NONE) {
                // Menu item has been pressed (we checked if the mouse click and release is on the same menu item)
                renderer->events.push(Event(EventType::MENU_SELECTION, sf::Vector2f(0.0,0.0)));
                if (menu_item_released_at == MenuType::LEAVE_GAME) {
                    // Leave the game and go back to the main menu (and do any cleanup)
                    renderer->world->EndWorld();
                    renderer->world = nullptr;
                    renderer->player = nullptr;

                    renderer->sound_manager->world = nullptr;
                    renderer->events.push(Event(EventType::ENTER_MAIN_MENU, sf::Vector2f(0.0,0.0)));

                    renderer->show_ingame_menu = false;
                    renderer->mouse_movement = true;

                    renderer->current_menu = MenuType::MAIN;
                    renderer->mode = GameMode::MENU;
                } else if (menu_item_released_at == MenuType::BACK) {
                    // Close the menu
                    renderer->show_ingame_menu = false;
                    renderer->mouse_movement = true;
                }
            }
        }
    }

    return true;
}

/**
 * Handles pending events for menu.
 */
bool InputHandler::MenuPollEvents(sf::Event& event) {
    // Check if any menu options are clicked
    static MenuType last_menu_item_clicked = MenuType::NONE;
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            // Check if mouse is clicking on any menu selection
            auto mouse_position = renderer->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            last_menu_item_clicked = renderer->MouseOverWhichMenuOption(mouse_position);
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto mouse_position = renderer->window->mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            auto menu_item_released_at = renderer->MouseOverWhichMenuOption(mouse_position);
            if (menu_item_released_at == last_menu_item_clicked and last_menu_item_clicked != MenuType::NONE) {
                // Menu item has been pressed (we checked if the mouse click and release is on the same menu item)
                renderer->events.push(Event(EventType::MENU_SELECTION, sf::Vector2f(0.0,0.0)));
                if (menu_item_released_at == MenuType::QUICK_MATCH) {
                    // Setup a quick match with some default settings
                    QuickMatchGame(16);
                } else if (menu_item_released_at == MenuType::EXIT_GAME) {
                    // Exit and close game
                    return false;
                } else if (menu_item_released_at == MenuType::VERSUS) {
                    renderer->current_menu = MenuType::VERSUS;
                    renderer->last_menu = MenuType::MAIN;
                } else if (menu_item_released_at == MenuType::BACK) {
                    renderer->current_menu = MenuType::MAIN;
                }
            }
        }
    }

    return true;
}

void InputHandler::QuickMatchGame(unsigned int const num_players) {
    // Quick Match clicked, setup a game for the player
    // Todo: Set this up into a function with customizable parameters
    // Generate random background map (regardless of map size, background map size is the same for simplicity)
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 random_generator(seed);
    std::uniform_real_distribution<> distribution_tilemap(0, 8);
    for (auto& column : renderer->background_map) {
        for (auto& entry : column) {
            entry = distribution_tilemap(random_generator);
        }
    }

    renderer->world = std::make_shared<World>();
    auto resource_player = renderer->world->AddResources(1000*10*10*2, 50*10*2, 500);
    resource_player->color = sf::Color::Green;

    // The first few players default to colors that are pleasing to the eye, and then from there random colors are given
    sf::Color player_colors[] = {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow,
                                 sf::Color::Cyan, sf::Color::Magenta};
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0, 255);
    unsigned char random_red = dis(gen);
    unsigned char random_green = dis(gen);
    unsigned char random_blue = dis(gen);

    renderer->player = renderer->world->AddPlayer();
    renderer->player->color = player_colors[0];
    renderer->player->position = sf::Vector2f(0.0, 0.0);
    renderer->player->name = "Test_Player";
    renderer->player->resources = 1500;
    renderer->player->CreateUnits(15, UnitType::BASE_LV1);

    for (unsigned int count = 0; count < num_players; ++count) {
        random_red = dis(gen);
        random_green = dis(gen);
        random_blue = dis(gen);

        auto enemy_player = renderer->world->AddPlayer();
        if (count < 4) {
            enemy_player->color = player_colors[count+1];
        } else {
            enemy_player->color = sf::Color(random_red, random_green, random_blue);
        }

        enemy_player->position = enemy_player->RandomWanderLocation();
        enemy_player->name = "Test_Computer_Player";
        enemy_player->ai_type = AiType::EASY;
        enemy_player->resources = 1000;
        enemy_player->CreateUnits(10, UnitType::BASE_LV1);
    }

    renderer->current_menu = MenuType::GAME_MENU;
    renderer->mode = GameMode::IN_GAME;

    renderer->sound_manager->world = renderer->world;
    renderer->events.push(Event(EventType::ENTER_GAME, sf::Vector2f(0.0,0.0)));
}