//
// Created by Austin on 5/3/2015.
//

#include "Renderer.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"
#include "Resources.hpp"
#include "SoundManager.hpp"

unsigned int ResolutionX = 800;
unsigned int ResolutionY = 600;

Renderer::Renderer()
    : mouse_movement(true)
    , current_menu(MenuType::MAIN)
    , last_menu(MenuType::NONE)
    , mode(GameMode::MENU)
    , show_ingame_menu(false)
    , music_on(true)
    , sound_on(true) {
    std::string title = "BubbleGrow V" + std::to_string(VERSION) + "." + std::to_string(SUB_VERSION);
    sf::ContextSettings settings;
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(ResolutionX, ResolutionY), title, sf::Style::Default, settings);
    //window->setFramerateLimit(60);

    // Scale view based on current resolution
    UpdateView(window->getSize());

    // Fonts
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
    textures[ImageId::BACKGROUND_MAIN_MENU] = sf::Texture();
    textures[ImageId::BACKGROUND_MAIN_MENU].loadFromFile("../../data/artwork/MainMenuBackground.png");
    textures[ImageId::BUBBLE] = sf::Texture();
    textures[ImageId::BUBBLE].loadFromFile("../../data/artwork/bubble.png");
    textures[ImageId::BUBBLE_TYPES] = sf::Texture();
    textures[ImageId::BUBBLE_TYPES].loadFromFile("../../data/artwork/Bubble_Types.png");
    textures[ImageId::PLAYER_SYMBOLS] = sf::Texture();
    textures[ImageId::PLAYER_SYMBOLS].loadFromFile("../../data/artwork/Player_Symbols.png");
    textures[ImageId::ARROW] = sf::Texture();
    textures[ImageId::ARROW].loadFromFile("../../data/artwork/arrow.png");
    textures[ImageId::HOTKEY_BAR] = sf::Texture();
    textures[ImageId::HOTKEY_BAR].loadFromFile("../../data/artwork/Bubble_Toolbar.png");

    // Initialize background and bubble batch drawer
    background_batch = BatchDrawer("../../data/artwork/Background_Tiles.png", 3, 3);
    bubbles_batch = BatchDrawer("../../data/artwork/Bubble_Types.png", 3, 3);
    symbols_batch = BatchDrawer("../../data/artwork/Player_Symbols.png", 3, 3);

    // Initiate main menu music
    events.push(Event(EventType::ENTER_MAIN_MENU, sf::Vector2f(0.0,0.0)));
}

/**
 * Scales view based on current resolution.
 */
void Renderer::UpdateView(sf::Vector2u new_window_size) {
    ResolutionX = new_window_size.x;
    ResolutionY = new_window_size.y;
    double scale_x = 1280.0 / new_window_size.x;
    double scale_y = 1024.0 / new_window_size.y;
    auto scale = scale_x > scale_y ? scale_x : scale_y;

    auto view = window->getDefaultView();
    view.zoom(scale);
    ResolutionX *= scale;
    ResolutionY *= scale;
    view.setSize(new_window_size.x*scale, new_window_size.y*scale);
    view.setCenter(new_window_size.x*scale/2.0, new_window_size.y*scale/2.0);
    window->setView(view);
}

/**
 * Render the game.
 */
void Renderer::RenderGame(double duration) {
    window->clear(sf::Color(243, 253, 243)); // Off-white/light green background

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
                                                player->position.x * kParallaxStrength,
                                                background_batch.sprite_pixel_height * scale * start_y -
                                                player->position.y * kParallaxStrength);
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
    sprites_symbols.clear();
    auto player_position = player->position;
    double scale = 0.1;
    std::vector<std::shared_ptr<Player>> non_resource_players;
    for (auto const& player_reference : world->players) {
        if (player_reference.second->type == PlayerType::RESOURCES) {
            // Draw Resources first since they are to displayed underneath everything else
            RenderPlayer(player_reference.second, player_position);
        } else if (RenderDistanceTo(player_reference.second->position) < ResolutionX *2) {
            // Only render players within view distance
            non_resource_players.push_back(player_reference.second);
        }
    }

    for (auto const& non_resource_player : non_resource_players) {
        RenderPlayer(non_resource_player, player_position);
    }

    bubbles_batch.UpdateEntries(sprites);
    window->draw(bubbles_batch);

    symbols_batch.UpdateEntries(sprites_symbols);
    window->draw(symbols_batch);
}

/**
 * Adds a unit for the given Player type to be drawn to the batch drawer.
 */
void Renderer::RenderUnit(std::shared_ptr<Unit> unit, PlayerType type, sf::Vector2f player_position, sf::Color color) {
    unsigned int sprite_index = 0;
    if (unit->type != UnitType::RESOURCE) {
        sprite_index = static_cast<unsigned int>(unit->type);
    }

    auto screen_location = sf::Vector2f((unit->position.x - player_position.x)*20.0 + ResolutionX /2,
                                        (unit->position.y - player_position.y)*20.0 + ResolutionY /2);
    double scale = unit->size/20.0;

    BatchEntry unit_sprite(screen_location, sprite_index, scale, color, true);
    sprites.push_back(unit_sprite);

    if (unit->type != UnitType::RESOURCE) {
        // Draw player's symbol on each unit (symbol color is a lighter shade of the bubble color)
        BatchEntry unit_symbol(screen_location, unit->symbol, scale, sf::Color((color.r+255)/2, (color.g+255)/2, (color.b+255)/2), true);
        sprites_symbols.push_back(unit_symbol);
    }
}

/**
 * Adds all units for given Player type to be drawn to the batch drawer.
 */
void Renderer::RenderPlayer(std::shared_ptr<Player> player_to_render, sf::Vector2f main_player_position) {
    for (auto const& unit : player_to_render->units) {
        if (player_to_render->type == PlayerType::RESOURCES and RenderDistanceTo(unit.second->position) > ResolutionX*2)
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
    RenderText("Resources: " + std::to_string(player->resources), sf::Vector2f(10.0, 0.0), TextSize::RESOURCE_COUNTER);
    RenderText("FPS: " + std::to_string(1.0 / duration), sf::Vector2f(10.0, 20.0), TextSize::RESOURCE_COUNTER);
    int distance_from_center = std::sqrt(
            player->position.x * player->position.x + player->position.y * player->position.y);
    RenderText("Distance from center: " + std::to_string(distance_from_center) + "/" +
                                                                                 std::to_string(static_cast<int>(world->map_radius)),
               sf::Vector2f(10.0, 40.0), TextSize::RESOURCE_COUNTER);

    // Render Bottom Hotkey Bar
    RenderImage(0.25, sf::Vector2f(0.0, 0.0), 0.0, sf::Color::White, ImageId::HOTKEY_BAR,
                sf::Vector2f(ResolutionX / 2, ResolutionY), false, true, true, false);

    if (show_ingame_menu) {
        current_menu = MenuType::GAME_MENU;
        RenderMenuText(current_menu);
    }

    if (world->players.size() <= 2 and player->units.size() > 0) {
        RenderText("WINNER!", sf::Vector2f(ResolutionX/2, ResolutionY/4), TextSize::GAME_MENU, sf::Color(52, 255, 102), true);
    } else if (player->units.size() == 0) {
        RenderText("GAME OVER", sf::Vector2f(ResolutionX/2, ResolutionY/4), TextSize::GAME_MENU, sf::Color(255, 52, 52), true);
    } else if (distance_from_center > world->map_radius) {
        RenderText("WARNING: Leaving game area!", sf::Vector2f(ResolutionX/2, ResolutionY/4), TextSize::WARNING, sf::Color(255, 70, 20), true);
    }
}

/**
 * Renders an image (identified by its ImageId) with the provided arguments.
 *
 * Note: The 4 default arguments are due to their necessary and regular use. Below is how to use them,
 *  - subtract_length: Used when you want to have an image at the right edge of the screen (with a position.x of ResolutionX
 *  - subtract_height: Same as above, but at the bottom of the screen
 *  - center_x: Centers the image on the x-axis at the position provided to the center of the image
 *  - center_y: Centers the image on the y-axis at the position provided to the center of the image
 */
void Renderer::RenderImage(double scale, sf::Vector2f origin, double rotation, sf::Color color, ImageId image_id, sf::Vector2f position,
                           bool subtract_length, bool subtract_height, bool center_x, bool center_y) {
    sf::Sprite image;
    image.setScale(scale, scale);
    image.setOrigin(origin.x,origin.y);
    image.setRotation(rotation);
    image.setColor(color);
    image.setTexture(textures[image_id]);

    if (subtract_length) {
        position.x -=  image.getLocalBounds().width*scale;
    }
    if (subtract_height) {
        position.y -=  image.getLocalBounds().height*scale;
    }
    if (center_x) {
        position.x -= image.getLocalBounds().width/(2.0/scale);
    }
    if (center_y) {
        position.y -= image.getLocalBounds().height/(2.0/scale);
    }

    image.setPosition(position.x, position.y);
    window->draw(image);
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
            auto player_screen_position = sf::Vector2f(ResolutionX /2, ResolutionY);
            auto target_screen_position = sf::Vector2f((player_reference.second->position.x - player->position.x)*20.0 + ResolutionX /2,
                                                       (player_reference.second->position.y - player->position.y)*20.0 + ResolutionY /2);
            double angle = std::atan2(target_screen_position.y - player_screen_position.y,
                                      target_screen_position.x - player_screen_position.x);

            auto distance_x = target_screen_position.x - player_screen_position.x;
            auto distance_y = target_screen_position.y - player_screen_position.y;
            auto render_distance = std::sqrt(distance_x*distance_x + distance_y*distance_y);
            if (render_distance > ResolutionX) {
                // Only render arrows for players that aren't within fighting distance (since you'll otherwise already
                // be seeing them on the screen).
                double magnitude;
                double abs_cos_angle = std::abs(std::cos(angle));
                double abs_sin_angle = std::abs(std::sin(angle));

                if (ResolutionX /2.0*abs_sin_angle <= ResolutionY /2.0*abs_cos_angle) {
                    magnitude = ResolutionX /2.0/abs_cos_angle;
                } else {
                    magnitude = ResolutionY /2.0/abs_sin_angle;
                }

                double x_position = magnitude*std::cos(angle) + ResolutionX /2;
                double y_position = magnitude*std::sin(angle) + ResolutionY /2;
                auto arrow_position = sf::Vector2f(x_position, y_position);

                double sprite_rotation;
                if (x_position > static_cast<double>(ResolutionX)-1.0) {
                    // Right wall
                    sprite_rotation = 90.0;
                } else if (x_position < 1.0) {
                    // Left wall
                    sprite_rotation = -90.0;
                } else if (y_position > static_cast<double>(ResolutionY)-1.0) {
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
    // Background and Render Logo
    RenderImage(0.45, sf::Vector2f(0.0, 0.0), 0.0, sf::Color::White, ImageId::BACKGROUND_MAIN_MENU,
                sf::Vector2f(ResolutionX/2, ResolutionY/2), false, false, true, true);
    RenderImage(0.4, sf::Vector2f(0.0, 0.0), 0.0, sf::Color::White, ImageId::LOGO,
                sf::Vector2f(ResolutionX/2, 75.0), false, false, true, false);

    // Render menu options
    RenderMenuText(current_menu);

    // Render toggle switches for sound and music
    std::string bool_string = "On";
    if (!sound_on) bool_string = "Off";
    sound_bounding_box = RenderText("Sound: " + bool_string, sf::Vector2f(10.0, ResolutionY - 60.0), TextSize::RESOURCE_COUNTER);
    if (!music_on) bool_string = "Off"; else bool_string = "On";
    music_bounding_box = RenderText("Music: " + bool_string, sf::Vector2f(10.0, ResolutionY - 40.0), TextSize::RESOURCE_COUNTER);
}

/**
 * Renders text at specified location and attributes.
 */
sf::FloatRect Renderer::RenderText(std::string display_text, sf::Vector2f location, unsigned int font_size, sf::Color color,
                                   bool center_text_x) {
    text.setString(display_text);
    text.setStyle(sf::Text::Regular);
    text.setColor(color);
    text.setCharacterSize(font_size);
    if (center_text_x) {
        text.setPosition(location.x - text.getLocalBounds().width/2.0, location.y);
    } else {
        text.setPosition(location);
    }
    //text.setOrigin(text.getLocalBounds().width/2.0, 0.0);
    text.setOrigin(0.0, 0.0);
    window->draw(text);

    return text.getGlobalBounds();
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
    y_offset += ResolutionY /2 - menu_height/2;

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
        menu_entry.setPosition(sf::Vector2f(ResolutionX /2, y_offset));
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
    auto player_screen_position = sf::Vector2f(ResolutionX /2, ResolutionY);
    auto target_screen_position = sf::Vector2f((destination.x - player->position.x)*20.0 + ResolutionX /2,
                                               (destination.y - player->position.y)*20.0 + ResolutionY /2);
    auto distance_x = target_screen_position.x - player_screen_position.x;
    auto distance_y = target_screen_position.y - player_screen_position.y;
    return std::sqrt(distance_x*distance_x + distance_y*distance_y);
}