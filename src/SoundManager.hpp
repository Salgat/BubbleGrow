//
// Created by Austin on 5/22/2015.
//

#ifndef BUBBLEGROW_SOUNDMANAGER_HPP
#define BUBBLEGROW_SOUNDMANAGER_HPP

#include "common.hpp"
#include "Event.hpp"

class Renderer;
class World;

/**
 * Handles playing events and other sounds based on queued events.
 */
class SoundManager {
private:
    std::map<SoundId, sf::SoundBuffer> sound_files; // Holds all loaded sound files

    sf::Music main_menu_music;
    sf::Music in_game_music;
    std::vector<sf::Sound> sounds; // Vector of available sounds to use (256 total)
    std::vector<bool> sound_availability; // If true, sound is available for use, other is in use. Loop through
                                          // to find an open sound.
    std::size_t reserved_sounds; // All numbers great than this value are reserved

    // Helper functions
    inline bool PlaySound(SoundId id, sf::Vector2f const& screen_position);
    inline bool PlaySound(Event const& event);
    inline void PlayEventSounds(std::stack<Event>& events);

public:
    double master_volume;
    double music_volume;

    std::shared_ptr<World> world;
    std::shared_ptr<Renderer> renderer;

    SoundManager(std::shared_ptr<Renderer> renderer);

    void ProcessPendingEvents();
};

#endif //BUBBLEGROW_SOUNDMANAGER_HPP
