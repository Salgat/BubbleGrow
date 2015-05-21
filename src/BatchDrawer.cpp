//
// Created by Austin on 5/20/2015.
//
// Note: The basic outline for this class is referenced from http://www.sfml-dev.org/tutorials/2.3/graphics-vertex-array.php
//

#include "BatchDrawer.hpp"

/**
 * Initialize with the sprite sheet to load and information about the sprite sheet.
 */
BatchDrawer::BatchDrawer(std::string sprite_sheet, unsigned int width, unsigned height)
    : spritesheet_width(width)
    , spritesheet_height(height)
    , center(false) {
    // Load the sprite sheet texture into memory and set vertices to handle square textures.
    spritesheet.loadFromFile(sprite_sheet);
    auto texture_size = spritesheet.getSize();
    sprite_pixel_width = texture_size.x / width;
    sprite_pixel_height = texture_size.y / height;

    vertices.setPrimitiveType(sf::Quads);
}

/**
 * Prepares BatchDrawer with information regarding which sprites from the sprite sheet to display and their locations
 * on the screen.
 */
void BatchDrawer::UpdateEntries(std::vector<BatchEntry> const& sprites) {
    // Vertices will need to hold a Quad (4 points) for every image to display
    vertices.resize(sprites.size() * 4);

    // Iterate through all sprites, setting up the images and their locations to display on the screen
    std::size_t index = 0;
    for (auto& sprite : sprites) {
        // Set pixel position on screen
        auto location = sprite.location;
        auto scale = sprite.scale;
        sf::Vertex* quad = &vertices[index*4];

        if (center) {
            // Center the position (offset back half the width and height)
            quad[0].position = sf::Vector2f(location.x - sprite_pixel_width*scale/2.0, location.y - sprite_pixel_height*scale/2.0);
            quad[1].position = sf::Vector2f(location.x + sprite_pixel_width*scale/2.0, location.y - sprite_pixel_height*scale/2.0);
            quad[2].position = sf::Vector2f(location.x + sprite_pixel_width*scale/2.0, location.y + sprite_pixel_height*scale/2.0);
            quad[3].position = sf::Vector2f(location.x - sprite_pixel_width*scale/2.0, location.y + sprite_pixel_height*scale/2.0);
        } else {
            quad[0].position = sf::Vector2f(location.x, location.y);
            quad[1].position = sf::Vector2f(location.x + sprite_pixel_width*scale, location.y);
            quad[2].position = sf::Vector2f(location.x + sprite_pixel_width*scale, location.y + sprite_pixel_height*scale);
            quad[3].position = sf::Vector2f(location.x, location.y + sprite_pixel_height*scale);
        }

        // Set texture coordinates (which image in sprite sheet to display)
        unsigned int row = sprite.index / spritesheet_width;
        unsigned int column = sprite.index % spritesheet_width;

        quad[0].texCoords = sf::Vector2f(row * sprite_pixel_width, column * sprite_pixel_height);
        quad[1].texCoords = sf::Vector2f((row+1) * sprite_pixel_width, column * sprite_pixel_height);
        quad[2].texCoords = sf::Vector2f((row+1) * sprite_pixel_width, (column+1) * sprite_pixel_height);
        quad[3].texCoords = sf::Vector2f((row) * sprite_pixel_width, (column+1) * sprite_pixel_height);

        // Set texture color
        auto color = sprite.color;
        quad[0].color = color;
        quad[1].color = color;
        quad[2].color = color;
        quad[3].color = color;

        ++index;
    }
}

void BatchDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Apply the transform
    states.transform *= getTransform();

    // Apply the tileset texture
    states.texture = &spritesheet;

    // Draw the vertex array
    target.draw(vertices, states);
}