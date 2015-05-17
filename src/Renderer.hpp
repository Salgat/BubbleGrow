//
// Created by Austin on 5/3/2015.
//

#ifndef BUBBLEGROW_RENDERER_HPP
#define BUBBLEGROW_RENDERER_HPP

#include "common.hpp"
#include "Request.hpp"

class World;
class Player;
class Unit;

/**
 * Renders game and handles user input.
 */
class Renderer {
private:
    bool mouse_movement;
    MenuType current_menu;
    MenuType last_menu; // This is only updated when a main menu selection includes a back button on the next menu

    bool show_ingame_menu; // Toggles in-game menu

    sf::Font font;
    sf::Text text;
    std::vector<std::pair<MenuType, sf::Text>> menu_text_entries; // Holds the sf::Text for all current menu options
    std::map<unsigned int, float> text_heights; // text_heights[font_size] = pixel_height

    std::vector<sf::Texture> background_tiles;
    std::size_t tile_count;
    std::array<std::array<std::size_t, tilemap_dimensions>, tilemap_dimensions> background_map;

    sf::Sprite sprite;
    std::map<ImageId, sf::Texture> textures;

    // Helper functions
    inline bool IsMouseOverText(sf::Text& text_object, sf::Vector2f cursor_location);
    inline MenuType MouseOverWhichMenuOption(sf::Vector2f cursor_location);

public:
    GameMode mode;

    std::shared_ptr<sf::RenderWindow> window;

    std::shared_ptr<World> world;
    std::shared_ptr<Player> player;

    Renderer();

    bool PollEvents();
    void ProcessInputs(); // Use player->requests to add input requests

    bool GamePollEvents(sf::Event& event);
    bool MenuPollEvents(sf::Event& event);

    void RenderGame();

    // Rendering functions for an active game world
    void RenderBackground();
    void RenderUnits();
    void RenderUnit(std::shared_ptr<Unit> unit, PlayerType type, sf::Vector2f player_position, sf::Color color);
    void RenderPlayer(std::shared_ptr<Player> player_to_render, sf::Vector2f main_player_position);
    //void RenderMiniMap();
    void RenderInterface();
    void RenderDirectionArrows();

    // Rendering functions for the menu
    void RenderMenu();
    void RenderText(std::string display_text, sf::Vector2f location, unsigned int font_size, sf::Color color = sf::Color::Black);
    void RenderMenuText(MenuType selection);


};

#endif //BUBBLEGROW_RENDERER_HPP

// Todo: Need to split up this class into smaller sections; possibly move input operations to their own class.