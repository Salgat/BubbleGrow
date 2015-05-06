//
// Created by Austin on 5/3/2015.
//

#include "World.hpp"
#include "Player.hpp"


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
        player.second->MakeDecisions();
    }

    // Execute request for each unit
    for (auto& player : players) {
        player.second->ProcessRequests(duration);
    }

    // Add and remove any new/expired units

}

std::shared_ptr<Player> World::AddPlayer() {
    auto player = std::make_shared<Player>();
    player->world = shared_from_this();
    player->id = ++player_id_count;
    players[player->id] = player;

    return player;
}



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

