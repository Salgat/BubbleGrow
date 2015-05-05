//
// Created by Austin on 5/3/2015.
//

#ifndef BUBBLEGROW_WORLD_HPP
#define BUBBLEGROW_WORLD_HPP

#include "common.hpp"

class Player;
class Unit;

/**
 * Manages the state of the entire game world.
 */
class World : public std::enable_shared_from_this<World> {
private:
    uint64_t player_id_count;

public:
    std::unordered_map<uint64_t, std::shared_ptr<Player>> players;

    World();

    void UpdateAndProcess(double duration);
    std::shared_ptr<Player> AddPlayer();
    std::shared_ptr<Unit> FindUnit(uint64_t owner_id, uint64_t unit_id);

};

#endif //BUBBLEGROW_WORLD_HPP
