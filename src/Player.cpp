//
// Created by Austin on 5/3/2015.
//

#include "Player.hpp"
#include "Unit.hpp"
#include "World.hpp"

Player::Player()
    : name("No Name")
    , id(0)
    , army_value(0)
    , resources(0)
    , unit_ids(0)
    , ai_type(AiType::NONE)
    , wander_range(20.0) {
    // Default to having no units
    for (auto& entry : number_of_units) {
        entry = 0;
    }
}

/**
 * Update the stats of all units owned by player.
 */
void Player::Update(double duration) {
    for (auto& unit : units) {
        unit.second->Update(duration);
    }
}

/**
 * Handle the requests provided.
 */
void Player::ProcessRequests(double duration) {
    // Todo: Parallelize
    for (auto& request : unit_requests) {
        auto unit = units.find(request.first)->second;
        unit->ProcessRequest(request.second, duration);
    }
}

/**
 * Process the decisions for each unit.
 */
void Player::MakeDecisions() {
    // Todo: Parallelize
    for (auto& unit : units) {
        unit_requests.find(unit.first)->second = unit.second->MakeDecision();
    }
}

/**
 * Creates new units based on given parameters. This function is executed in a sequential manner (for thread-safety).
 */
void Player::CreateUnits(int amount, UnitType type) {
    if (type == UnitType::BASE) {
        auto resource_cost = amount * BASE_RESOURCE_COST;
        if (resource_cost > resources) {
            // Can't afford to create requested amount, create maximum we can afford
            amount = resources % BASE_RESOURCE_COST;
        }

        while (amount-- > 0) {
            // Initialize a new unit
            resources -= BASE_RESOURCE_COST;
            auto new_unit = std::make_shared<Unit>();
            new_unit->position = position;
            new_unit->id = ++unit_ids;
            new_unit->owner_id = id;
            new_unit->owner = shared_from_this();
            new_unit->world = world;

            // Add to player's maps
            units[new_unit->id] = new_unit;
            unit_requests[new_unit->id] = Request();
            unit_requests[new_unit->id].type = RequestType::NONE;
        }
    }
}
