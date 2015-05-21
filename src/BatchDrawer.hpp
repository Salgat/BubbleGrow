//
// Created by Austin on 5/20/2015.
//

#ifndef BUBBLEGROW_BATCHDRAWER_HPP
#define BUBBLEGROW_BATCHDRAWER_HPP

#include "common.hpp"

/**
 * Defines a sprite that is to be drawn in the BatchDrawer class.
 */
class BatchEntry {
public:
    sf::Vector2f location;
    unsigned int index; // The sprite entry in the sprite sheet
    double scale;
    sf::Color color; // Color filter to apply to texture
    bool center;

    BatchEntry()
        : location(sf::Vector2f(0.0, 0.0))
        , index(0)
        , scale(1.0)
        , color(sf::Color::White)
        , center(false)  {

    }

    BatchEntry(sf::Vector2f screen_location, unsigned int spritesheet_index, double scaled_value = 1.0, sf::Color color_filter = sf::Color::White, bool centered = false)
        : location(screen_location)
        , index(spritesheet_index)
        , scale(scaled_value)
        , color(color_filter)
        , center(centered) {

    }
};

/**
 * Draws all the provided sprites, sharing the same sprite sheet, in a single batch, reducing draw calls to a single one.
 */
class BatchDrawer : public sf::Drawable, public sf::Transformable {
private:
    sf::VertexArray vertices;
    sf::Texture spritesheet;
    unsigned int spritesheet_width; // Number of pictures per row
    unsigned int spritesheet_height; // Number of pictures per column

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    bool center;
    unsigned int sprite_pixel_width; // Pixel width of a single sprite entry in the spritesheet
    unsigned int sprite_pixel_height; // Pixel height

    BatchDrawer() {}
    BatchDrawer(std::string sprite_sheet, unsigned int width, unsigned height);

    void UpdateEntries(std::vector <BatchEntry> const &sprites);
};

#endif //BUBBLEGROW_BATCHDRAWER_HPP
