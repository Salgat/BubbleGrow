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
    , type (PlayerType::PLAYER)
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
        if (unit.second->health[0] > 0)
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
        if (unit->health[0] > 0)
            unit->ProcessRequest(request.second, duration);
    }
}

/**
 * Process the decisions for each unit.
 */
void Player::MakeDecisions() {
    // Todo: Parallelize
    for (auto& unit : units) {
        if (unit.second->health[0] > 0)
            unit.second->MakeDecision(unit_requests.find(unit.first)->second);
    }
}

/**
 * Removes from the units map any expired units (usually because they are dead).
 */
void Player::RemoveExpiredUnits() {
    std::stack<uint64_t> units_to_remove;
    for (auto& unit : units) {
        if (unit.second->health[0] <= 0) {
            units_to_remove.push(unit.first);
            number_of_units[static_cast<std::size_t>(unit.second->type)] -= 1;
        }
    }

    while(!units_to_remove.empty()) {
        units.erase(units_to_remove.top());
        unit_requests.erase(units_to_remove.top());
        units_to_remove.pop();
    }
}

/**
 * Creates new units based on given parameters. This function is executed in a sequential manner (for thread-safety).
 */
void Player::CreateUnits(int amount, UnitType type) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> distribution_radius(0, wander_range);
    std::uniform_real_distribution<> distribution_angle(0, 2*PI);

    if (type == UnitType::BASE) {
        auto resource_cost = amount * kBaseResourceCost;
        if (resource_cost > resources) {
            // Can't afford to create requested amount, create maximum we can afford
            amount = resources / kBaseResourceCost;
        }

        number_of_units[static_cast<std::size_t>(type)] += amount;
        while (amount-- > 0) {
            double random_radius = distribution_radius(gen);
            double random_angle = distribution_angle(gen);

            // Initialize a new unit
            resources -= kBaseResourceCost;
            auto new_unit = std::make_shared<Unit>();
            new_unit->position = sf::Vector2f(position.x + static_cast<float>(random_radius*std::cos(random_angle)),
                                              position.y + static_cast<float>(random_radius*std::sin(random_angle)));
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
