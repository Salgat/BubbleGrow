//
// Created by Austin on 5/20/2015.
//

#ifndef BUBBLEGROW_BATCHDRAWER_HPP
#define BUBBLEGROW_BATCHDRAWER_HPP

#include "common.hpp"

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
    unsigned int sprite_pixel_width; // Pixel width of a single sprite entry in the spritesheet
    unsigned int sprite_pixel_height; // Pixel height

    BatchDrawer() {}
    BatchDrawer(std::string sprite_sheet, unsigned int width, unsigned height);

    void UpdateTextures(std::vector<std::tuple<sf::Vector2f, unsigned int, double>> const& sprites);
};

#endif //BUBBLEGROW_BATCHDRAWER_HPP
