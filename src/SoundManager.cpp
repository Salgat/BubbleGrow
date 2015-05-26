//
// Created by Austin on 5/22/2015.
//

#include "SoundManager.hpp"
#include "Event.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Unit.hpp"
#include "Renderer.hpp"

SoundManager::SoundManager(std::shared_ptr<Renderer> renderer_)
    : master_volume(1.0)
    , music_volume(1.0) {
    std::cout << "Calling constructor" << std::endl;
    // Setup the 256 sound channels to use
    for (std::size_t index = 0; index < 256; ++index) {
        sounds.push_back(sf::Sound());
        sound_availability.push_back(true);
    }
    std::cout << "Loading sounds" << std::endl;
    // Load sound files
    sound_files[SoundId::BUBBLE_DIE] = sf::SoundBuffer();
    sound_files[SoundId::BUBBLE_DIE].loadFromFile("../../data/sounds/Bubble_Die.ogg");
    sound_files[SoundId::BUBBLE_ATTACK] = sf::SoundBuffer();
    sound_files[SoundId::BUBBLE_ATTACK].loadFromFile("../../data/sounds/Bubble_Attack.ogg");
    sound_files[SoundId::BUBBLE_GATHER] = sf::SoundBuffer();
    sound_files[SoundId::BUBBLE_GATHER].loadFromFile("../../data/sounds/Bubble_Gather.ogg");
    sound_files[SoundId::MENU_SELECTION] = sf::SoundBuffer();
    sound_files[SoundId::MENU_SELECTION].loadFromFile("../../data/sounds/Menu_Selection.ogg");
    sound_files[SoundId::MENU_SELECTION_CHANGE] = sf::SoundBuffer();
    sound_files[SoundId::MENU_SELECTION_CHANGE].loadFromFile("../../data/sounds/Menu_Selection_Change.ogg");
    //sound_files[SoundId::MAIN_MENU_MUSIC] = sf::SoundBuffer();
    //sound_files[SoundId::MAIN_MENU_MUSIC].loadFromFile("../../data/sounds/Main_Menu_Music.ogg");
    //sound_files[SoundId::IN_GAME_MUSIC] = sf::SoundBuffer();
    //sound_files[SoundId::IN_GAME_MUSIC].loadFromFile("../../data/sounds/In_Game_Music.ogg");
    main_menu_music.openFromFile("../../data/sounds/Main_Menu_Music.ogg");
    main_menu_music.setLoop(true);
    in_game_music.openFromFile("../../data/sounds/In_Game_Music.ogg");
    in_game_music.setLoop(true);
    std::cout << "Sounds loaded" << std::endl;
    // Reserve and setup certain channels for a specific use
    sound_availability[255] = false;
    sounds[255].setLoop(true);
    sound_availability[254] = false;
    sounds[254].setLoop(true);

    reserved_sounds = 254;

    // Set position and direction of listener (all sounds are relative to the position of the player)
    sf::Listener::setPosition(0.0, 0.0, 0.0);
    sf::Listener::setDirection(0.0, -1.0, 0.0);

    renderer = renderer_;
}

/**
 * Plays the specific SoundId at the position provided (for stereo sound).
 */
bool SoundManager::PlaySound(SoundId id, sf::Vector2f const& screen_position) {
    // Music is treated special
    if (id == SoundId::IN_GAME_MUSIC) {
        main_menu_music.stop();
        in_game_music.setVolume(20.0*master_volume);
        in_game_music.play();
    } else if (id == SoundId::MAIN_MENU_MUSIC) {
        in_game_music.stop();
        main_menu_music.setVolume(20.0*master_volume);
        main_menu_music.play();
    } else if (sound_files.count(id) > 0) {
        for (std::size_t index = 0; index < reserved_sounds; ++index) {
            if (sound_availability[index] and screen_position.x < 2*ResolutionX and screen_position.y < 2*ResolutionY
                                          and screen_position.x > -2*static_cast<double>(ResolutionX) and screen_position.y > -2*static_cast<double>(ResolutionY)) {
                // Open channel found
                sounds[index].setBuffer(sound_files[id]);
                sounds[index].setPosition((screen_position.x-ResolutionX/2.0)/1000.0, (screen_position.y-ResolutionY/2.0)/1000.0, 0.0);
                sounds[index].setVolume(15.0*master_volume);
                sounds[index].play();
                sound_availability[index] = false;
                return true;
            }
        }
    }

    return false;
}

/**
 * Calls PlaySound(SoundId, position) based on event details.
 */
bool SoundManager::PlaySound(Event const& event) {
    SoundId sound_to_play = SoundId::NONE;
    if (event.type == EventType::BUBBLE_ATTACK) {
        sound_to_play = SoundId::BUBBLE_ATTACK;
    } else if (event.type == EventType::BUBBLE_GATHER) {
        sound_to_play = SoundId::BUBBLE_GATHER;
    } else if (event.type == EventType::BUBBLE_DIE) {
        sound_to_play = SoundId::BUBBLE_DIE;
    } else if (event.type == EventType::MENU_SELECTION) {
        sound_to_play = SoundId::MENU_SELECTION;
    } else if (event.type == EventType::MENU_SELECTION_CHANGE) {
        sound_to_play = SoundId::MENU_SELECTION_CHANGE;
    } else if (event.type == EventType::ENTER_GAME) {
        sound_to_play = SoundId::IN_GAME_MUSIC;
    } else if (event.type == EventType::ENTER_MAIN_MENU) {
        sound_to_play = SoundId::MAIN_MENU_MUSIC;
    }

    // Convert event position from game to screen coordinates
    sf::Vector2f screen_position(ResolutionX/2.0, ResolutionY/2.0);
    if (renderer->player) {
        screen_position = sf::Vector2f((event.position.x - renderer->player->position.x)*20.0 + ResolutionX /2,
                                       (event.position.y - renderer->player->position.y)*20.0 + ResolutionY /2);
    }

    PlaySound(sound_to_play, screen_position);
}

/**
 * Goes through world and renderer to find any pending events to play sounds for.
 */
void SoundManager::ProcessPendingEvents() {
    // Iterate through all players and units finding any events to play sounds for
    if (world) {
        for (auto& player : world->players) {
            // Play Player Events
            PlayEventSounds(player.second->events);

            // Play Unit Events
            for (auto& unit : player.second->units) {
                PlayEventSounds(unit.second->events);
            }
        }
    }

    PlayEventSounds(renderer->events);

    // Check if any sounds are done, then free up the channel
    for (std::size_t index = 0; index < reserved_sounds; ++index) {
        if (!sound_availability[index] and sounds[index].getStatus() == sf::SoundSource::Status::Stopped) {
            sound_availability[index] = true;
        }
    }
}

/**
 * Plays sounds from a given event stack.
 */
void SoundManager::PlayEventSounds(std::stack<Event>& events) {
    while (!events.empty()) {
        PlaySound(events.top());
        events.pop();
    }
}