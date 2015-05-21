//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"
#include "Resources.hpp"

Renderer::Renderer()
    : mouse_movement(true)
    , current_menu(MenuType::MAIN)
    , last_menu(MenuType::NONE)
    , mode(GameMode::MENU)
    , show_ingame_menu(false) {
    std::string title = "BubbleGrow V" + std::to_string(VERSION) + "." + std::to_string(SUB_VERSION);
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y), title, sf::Style::Default, settings);
    //window->setFramerateLimit(60);

    if (!font.loadFromFile("../../data/fonts/Sile.ttf")) {
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
    textures[ImageId::LOGO] = sf::Texture();
    textures[ImageId::LOGO].loadFromFile("../../data/artwork/logo.png");
    textures[ImageId::BUBBLE] = sf::Texture();
    textures[ImageId::BUBBLE].loadFromFile("../../data/artwork/bubble.png");
    textures[ImageId::BUBBLE_TYPES] = sf::Texture();
    textures[ImageId::BUBBLE_TYPES].loadFromFile("../../data/artwork/Bubble_Types.png");
    textures[ImageId::PLAYER_SYMBOLS] = sf::Texture();
    textures[ImageId::PLAYER_SYMBOLS].loadFromFile("../../data/artwork/Player_Symbols.png");
    textures[ImageId::ARROW] = sf::Texture();
    textures[ImageId::ARROW].loadFromFile("../../data/artwork/arrow.png");

    // Generate random background map (regardless of map size, background map size is the same for simplicity)
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 random_generator(seed);
    std::uniform_real_distribution<> distribution_tilemap(0, kTileStrings.size());
    for (auto& column : background_map) {
        for (auto& entry : column) {
            entry = distribution_tilemap(random_generator);
        }
    }

    // Initialize background and bubble batch drawer
    background_batch = BatchDrawer("../../data/artwork/BG_Tile1.png", 1, 1);
    bubbles_batch = BatchDrawer("../../data/artwork/Bubble_Types.png", 3, 3);
    bubbles_batch.center = true;
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
            player->PlayerPurchaseRequest(10, UnitType::BASE_LV1);
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
                    auto resource_player = world->AddResources(1000*10*10*2, 50*10*2, 500);
                    resource_player->color = sf::Color::Green;

                    sf::Color player_colors[] = {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow,
                                                 sf::Color::Cyan, sf::Color::Magenta};

                    player = world->AddPlayer();
                    player->color = player_colors[0];
                    player->position = sf::Vector2f(0.0, 0.0);
                    player->name = "Test_Player";
                    player->resources = 1000;
                    player->CreateUnits(10, UnitType::BASE_LV1);

                    for (unsigned int count = 0; count < 4; ++count) {
                        auto enemy_player = world->AddPlayer();
                        enemy_player->color = player_colors[count+1];
                        enemy_player->position = enemy_player->RandomWanderLocation();
                        enemy_player->name = "Test_Computer_Player";
                        enemy_player->ai_type = AiType::EASY;
                        enemy_player->resources = 1000;
                        enemy_player->CreateUnits(10, UnitType::BASE_LV1);
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
void Renderer::RenderGame(double duration) {
    window->clear(sf::Color(255, 255, 255));

    // Render based on if the player is using the menu (no active game) or playing a game
    if (mode == GameMode::IN_GAME) {
        RenderBackground();
        RenderUnits();
        RenderInterface(duration);
    } else if (mode == GameMode::MENU) {
        RenderMenu();
    }

    window->display();
}

/**
 * Render the background for the game.
 */
void Renderer::RenderBackground() {
    auto player_position = player->position;

    double scale = 0.7;
    int start_x = -1 * static_cast<int>(tilemap_dimensions)/2;
    int start_y = -1 * static_cast<int>(tilemap_dimensions)/2;

    // Go through and draw each tile, which is offset by its position in background_map
    sprites.clear();
    for (auto const& column : background_map) {
        for (auto const entry : column) {
            auto screen_location = sf::Vector2f(background_batch.sprite_pixel_width * scale * start_x -
                                                player->position.x * parallax_strength,
                                                background_batch.sprite_pixel_height * scale * start_y -
                                                player->position.y * parallax_strength);
            unsigned int sprite_index = entry;
            BatchEntry background_sprite(screen_location, sprite_index, scale);
            sprites.push_back(background_sprite);

            ++start_y;
        }
        ++start_x;
        start_y = -1 * static_cast<int>(tilemap_dimensions)/2;
    }

    background_batch.UpdateEntries(sprites);
    window->draw(background_batch);
}

/**
 * Render the units for the game.
 */
void Renderer::RenderUnits() {
    if (!player or !world)
        return;

    sprites.clear();
    auto player_position = player->position;
    double scale = 0.1;
    std::vector<std::shared_ptr<Player>> non_resource_players;
    for (auto const& player_reference : world->players) {
        if (player_reference.second->type == PlayerType::RESOURCES) {
            // Draw Resources first since they are to displayed underneath everything else
            RenderPlayer(player_reference.second, player_position);
        } else if (RenderDistanceTo(player_reference.second->position) < RESOLUTION_X*2) {
            // Only render players within view distance
            non_resource_players.push_back(player_reference.second);
        }
    }

    for (auto const& non_resource_player : non_resource_players) {
        RenderPlayer(non_resource_player, player_position);
    }

    bubbles_batch.UpdateEntries(sprites);
    window->draw(bubbles_batch);
}

/**
 * Adds a unit for the given Player type to be drawn to the batch drawer.
 */
void Renderer::RenderUnit(std::shared_ptr<Unit> unit, PlayerType type, sf::Vector2f player_position, sf::Color color) {
    unsigned int sprite_index = 0;
    if (unit->type != UnitType::RESOURCE) {
        sprite_index = static_cast<unsigned int>(unit->type);
    }

    auto screen_location = sf::Vector2f((unit->position.x - player_position.x)*20.0 + RESOLUTION_X/2,
                                        (unit->position.y - player_position.y)*20.0 + RESOLUTION_Y/2);
    double scale = unit->size/20.0;

    BatchEntry unit_sprite(screen_location, sprite_index, scale, color, true);
    sprites.push_back(unit_sprite);
}

/**
 * Adds all units for given Player type to be drawn to the batch drawer.
 */
void Renderer::RenderPlayer(std::shared_ptr<Player> player_to_render, sf::Vector2f main_player_position) {
    for (auto const& unit : player_to_render->units) {
        if (player_to_render->type == PlayerType::RESOURCES and RenderDistanceTo(unit.second->position) > RESOLUTION_X*2)
            continue;

        RenderUnit(unit.second, player_to_render->type, main_player_position, player_to_render->color);
    }
}

/**
 * Render the interface for in-game.
 */
void Renderer::RenderInterface(double duration) {
    // Display arrows pointing to other players
    RenderDirectionArrows();

    // Display resource count
    RenderText("Resources: " + std::to_string(player->resources), sf::Vector2f(10.0, 10.0), TextSize::RESOURCE_COUNTER);
    RenderText("FPS: " + std::to_string(1.0/duration), sf::Vector2f(10.0, 30.0), TextSize::RESOURCE_COUNTER);

    if (show_ingame_menu) {
        current_menu = MenuType::GAME_MENU;
        RenderMenuText(current_menu);
    }
}

/**
 * Renders arrows that indicate direction of other players.
 */
void Renderer::RenderDirectionArrows() {
    sf::Sprite arrow_sprite;
    arrow_sprite.setTexture(textures[ImageId::ARROW]);
    arrow_sprite.setScale(0.2, 0.2);
    for (auto const& player_reference : world->players) {
        if (player_reference.second->type == PlayerType::PLAYER and player_reference.second != player) {
            // Determine the distance and angle to the player and scale the distance of the arrow by the distance
            auto player_screen_position = sf::Vector2f(RESOLUTION_X/2, RESOLUTION_Y);
            auto target_screen_position = sf::Vector2f((player_reference.second->position.x - player->position.x)*20.0 + RESOLUTION_X/2,
                                                       (player_reference.second->position.y - player->position.y)*20.0 + RESOLUTION_Y/2);
            double angle = std::atan2(target_screen_position.y - player_screen_position.y,
                                      target_screen_position.x - player_screen_position.x);

            auto distance_x = target_screen_position.x - player_screen_position.x;
            auto distance_y = target_screen_position.y - player_screen_position.y;
            auto render_distance = std::sqrt(distance_x*distance_x + distance_y*distance_y);
            if (render_distance > RESOLUTION_X) {
                // Only render arrows for players that aren't within fighting distance (since you'll otherwise already
                // be seeing them on the screen).
                double magnitude;
                double abs_cos_angle = std::abs(std::cos(angle));
                double abs_sin_angle = std::abs(std::sin(angle));

                if (RESOLUTION_X/2.0*abs_sin_angle <= RESOLUTION_Y/2.0*abs_cos_angle) {
                    magnitude = RESOLUTION_X/2.0/abs_cos_angle;
                } else {
                    magnitude = RESOLUTION_Y/2.0/abs_sin_angle;
                }

                double x_position = magnitude*std::cos(angle) + RESOLUTION_X/2;
                double y_position = magnitude*std::sin(angle) + RESOLUTION_Y/2;
                auto arrow_position = sf::Vector2f(x_position, y_position);

                double sprite_rotation;
                if (x_position > static_cast<double>(RESOLUTION_X)-1.0) {
                    // Right wall
                    sprite_rotation = 90.0;
                } else if (x_position < 1.0) {
                    // Left wall
                    sprite_rotation = -90.0;
                } else if (y_position > static_cast<double>(RESOLUTION_Y)-1.0) {
                    // Bottom wall
                    sprite_rotation = 180.0;
                } else {
                    // Top wall
                    sprite_rotation = 0.0;
                }

                arrow_sprite.setColor(player_reference.second->color);
                arrow_sprite.setOrigin(arrow_sprite.getLocalBounds().width/2.0, 0.0);
                arrow_sprite.setPosition(arrow_position);
                arrow_sprite.setRotation(sprite_rotation);

                window->draw(arrow_sprite);
            }
        }
    }
}

/**
 * Renders the start menu (not the in-game menu).
 */
void Renderer::RenderMenu() {
    // Render Logo
    sprite.setOrigin(0.0,0.0);
    sprite.setRotation(0.0);
    sprite.setColor(sf::Color::White);
    sprite.setTexture(textures[ImageId::LOGO]);
    double scale = 0.4;
    sprite.setScale(scale, scale);
    sprite.setPosition(RESOLUTION_X/2 - sprite.getLocalBounds().width/(2.0/scale), 75.0);
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
        //menu_entry.setColor(sf::Color::Black);
        menu_entry.setColor(sf::Color(0, 0, 50));
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

/**
 * Returns pixel distance from player position to destination (provided in game world coordinates).
 */
inline double Renderer::RenderDistanceTo(sf::Vector2f destination) {
    auto player_screen_position = sf::Vector2f(RESOLUTION_X/2, RESOLUTION_Y);
    auto target_screen_position = sf::Vector2f((destination.x - player->position.x)*20.0 + RESOLUTION_X/2,
                                               (destination.y - player->position.y)*20.0 + RESOLUTION_Y/2);
    auto distance_x = target_screen_position.x - player_screen_position.x;
    auto distance_y = target_screen_position.y - player_screen_position.y;
    return std::sqrt(distance_x*distance_x + distance_y*distance_y);
}