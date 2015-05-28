//
// Created by Austin on 5/3/2015.
//

#ifndef BUBBLEGROW_RENDERER_HPP
#define BUBBLEGROW_RENDERER_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Event.hpp"
#include "BatchDrawer.hpp"

class World;
class Player;
class Unit;
class SoundManager;
class InputHandler;

/**
 * Renders game and handles user input.
 */
class Renderer {
    friend class InputHandler;

private:
    bool mouse_movement;
    MenuType current_menu;
    MenuType last_menu; // This is only updated when a main menu selection includes a back button on the next menu

    bool show_ingame_menu; // Toggles in-game menu

    sf::Font font;
    sf::Text text;
    sf::FloatRect music_bounding_box;
    sf::FloatRect sound_bounding_box;
    std::vector<std::pair<MenuType, sf::Text>> menu_text_entries; // Holds the sf::Text for all current menu options
    std::map<unsigned int, float> text_heights; // text_heights[font_size] = pixel_height

    std::array<std::array<std::size_t, tilemap_dimensions>, tilemap_dimensions> background_map;
    BatchDrawer background_batch;
    BatchDrawer bubbles_batch;
    BatchDrawer symbols_batch;
    std::vector<BatchEntry> sprites;
    std::vector<BatchEntry> sprites_symbols;

    std::map<ImageId, sf::Texture> textures;

    // Helper functions
    void UpdateView(sf::Vector2u new_window_size);
    inline bool IsMouseOverText(sf::Text& text_object, sf::Vector2f cursor_location);
    MenuType MouseOverWhichMenuOption(sf::Vector2f cursor_location);
    inline double RenderDistanceTo(sf::Vector2f destination);
    inline void RenderImage(double scale, sf::Vector2f origin, double rotation, sf::Color color, ImageId image_id,
                            sf::Vector2f position, bool subtract_length = false, bool subtract_height = false,
                            bool center_x = false, bool center_y = false);

public:
    GameMode mode;

    std::shared_ptr<sf::RenderWindow> window;

    std::shared_ptr<World> world;
    std::shared_ptr<Player> player;
    std::shared_ptr<SoundManager> sound_manager;

    std::stack<Event> events;
    bool music_on;
    bool sound_on;

    Renderer();

    // Rendering functions for an active game world
    void RenderGame(double duration);
    void RenderBackground();
    void RenderUnits();
    void RenderUnit(std::shared_ptr<Unit> unit, PlayerType type, sf::Vector2f player_position, sf::Color color);
    void RenderPlayer(std::shared_ptr<Player> player_to_render, sf::Vector2f main_player_position);
    //void RenderMiniMap();
    void RenderInterface(double duration);
    void RenderDirectionArrows();

    // Rendering functions for the menu
    void RenderMenu();
    sf::FloatRect RenderText(std::string display_text, sf::Vector2f location, unsigned int font_size, sf::Color color = sf::Color::Black);
    void RenderMenuText(MenuType selection);


};

#endif //BUBBLEGROW_RENDERER_HPP

// Todo: Need to split up this class into smaller sections; possibly move input operations to their own class.