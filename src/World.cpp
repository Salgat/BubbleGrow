//
// Created by Austin on 5/3/2015.
//

#include "World.hpp"
#include "Player.hpp"
#include "Resources.hpp"


World::World()
    : player_id_count(0) {

}

/**
 * Processes the game for the given duration, including unit AI, user input, request processing, etc.
 */
void World::UpdateAndProcess(double duration) {
    //std::cout << "Update Frequency: " << 1.0/duration << std::endl;

    // Update each unit
    for (auto& player : players) {
        player.second->Update(duration);
    }

    // Have each unit make a decision
    for (auto& player : players) {
        if (player.second->type != PlayerType::RESOURCES)
            player.second->MakeDecisions();
    }

    // Execute request for each unit
    for (auto& player : players) {
        if (player.second->type != PlayerType::RESOURCES)
            player.second->ProcessRequests(duration);
    }

    // Add and remove any new/expired units

}

/**
 * Adds and initializes a player to the world.
 *
 * Todo: Add parameters to define if player is computer controlled and AI level.
 */
std::shared_ptr<Player> World::AddPlayer() {
    auto player = std::make_shared<Player>();
    player->world = shared_from_this();
    player->id = ++player_id_count;
    players[player->id] = player;

    return player;
}

/**
 * Adds and initializes resources to the map.
 */
std::shared_ptr<Resources> World::AddResources(unsigned int amount, float radius, float density) {
    auto resource_player = std::make_shared<Resources>();
    resource_player->world = shared_from_this();
    resource_player->id = ++player_id_count;
    players[resource_player->id] = resource_player;

    resource_player->CreateResources(amount, radius, density);

    return resource_player;
}

/**
 * Finds a unit based on its owner and unit id, returning a reference to that unit.
 */
std::shared_ptr<Unit> World::FindUnit(uint64_t owner_id, uint64_t unit_id) {
    for (auto& player : players) {
        if (player.first == owner_id) {
            auto unit = player.second->units.find(unit_id);
            if (unit != player.second->units.end())
                return unit->second;
        }
    }

    return nullptr;
}

