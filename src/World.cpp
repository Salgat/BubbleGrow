//
// Created by Austin on 5/3/2015.
//

#include "World.hpp"
#include "Player.hpp"
#include "Resources.hpp"


World::World()
    : player_id_count(0)
    , map_radius(800.0) {

}

/**
 * Processes the game for the given duration, including unit AI, user input, request processing, etc.
 */
void World::UpdateAndProcess(double duration) {
    // Execute requests for player
    #pragma omp parallel
    #pragma omp single
    {
        for(auto it = players.begin(); it != players.end(); ++it)
                #pragma omp task firstprivate(it)
                it->second->ProcessPlayerRequests(duration);
        #pragma omp taskwait
    }

    // Update each unit
    #pragma omp parallel
    #pragma omp single
    {
        for(auto it = players.begin(); it != players.end(); ++it)
                #pragma omp task firstprivate(it)
                it->second->Update(duration);
        #pragma omp taskwait
    }

    // Have each unit make a decision
    #pragma omp parallel
    #pragma omp single
    {
        for(auto it = players.begin(); it != players.end(); ++it)
            if (it->second->type != PlayerType::RESOURCES) {
                #pragma omp task firstprivate(it)
                it->second->MakeDecisions(duration);
            }
        #pragma omp taskwait
    }

    // Execute request for each unit
    #pragma omp parallel
    #pragma omp single
    {
        for(auto it = players.begin(); it != players.end(); ++it)
            if (it->second->type != PlayerType::RESOURCES) {
                #pragma omp task firstprivate(it)
                it->second->ProcessRequests(duration);
            }
        #pragma omp taskwait
    }

    // Add and remove any new/expired units
    std::vector<uint64_t> players_to_remove;
    for (auto& player : players) {
        player.second->RemoveExpiredUnits();
        if (player.second->units.size() == 0) {
            players_to_remove.push_back(player.first);
        }
    }

    for (auto& player : players_to_remove) {
        players.erase(player);
    }
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
            if (unit != player.second->units.end()) {
                return unit->second;
            } else{
                break;
            }
        }
    }

    return nullptr;
}