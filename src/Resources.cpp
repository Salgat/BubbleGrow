//
// Created by Austin on 5/5/2015.
//

#include "Resources.hpp"
#include "Unit.hpp"

Resources::Resources() {
    unit_ids = 0;
    type = PlayerType::RESOURCES;
    ai_type = AiType::NONE;
    wander_range = 0.0;
    resources = 0;

    // Default to having no units
    for (auto& entry : number_of_units) {
        entry = 0;
    }
}

/**
 * Updates resource units based on how much of its resource count was consumed.
 */
void Resources::Update(double duration) {
    unsigned int updated_resources = 0;
    for (auto& unit : units) {
        // Update total resource count
        unit.second->size = unit.second->resource_value/6.0;
        updated_resources += unit.second->resource_value;
    }
    resources = updated_resources;
}

/**
 * Removes units that no longer have any resources.
 */
void Resources::RemoveExpiredUnits() {
    std::stack<uint64_t> units_to_remove;
    for (auto& unit : units) {
        if (unit.second->resource_value <= 0) {
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
 * Creates resources on the map randomly distributed from (0.0,0.0) to radius, scaled by density (resources/m^2).
 */
void Resources::CreateResources(unsigned int amount, float range, float density) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> distribution_range(-1.0* range, range);
    std::uniform_real_distribution<> distribution_angle(0, 2*PI);
    // The resource amount of each unit is inversely proportional to area.
    std::uniform_real_distribution<> distribution_amount(0, 0.5 * amount * density / (range * range));

    int current_amount = amount;
    while (current_amount > 0) {
        // Initialize a new resource unit
        double random_x = distribution_range(gen);
        double random_y = distribution_range(gen);
        double random_angle = distribution_angle(gen);
        unsigned int random_amount = distribution_amount(gen);
        current_amount -= random_amount;

        auto new_unit = std::make_shared<Unit>();
        new_unit->main_type = UnitMainType::RESOURCE;
        new_unit->type = UnitType::RESOURCE;
        new_unit->resource_value = random_amount;
        new_unit->size = random_amount/6.0;
        //new_unit->position = sf::Vector2f(static_cast<float>(random_radius*std::cos(random_angle)),
        //                                  static_cast<float>(random_radius*std::sin(random_angle)));
        new_unit->position = sf::Vector2f(random_x, random_y);
        new_unit->id = ++unit_ids;
        new_unit->owner_id = id;
        new_unit->owner = shared_from_this();
        new_unit->world = world;

        // Add to player's maps
        units[new_unit->id] = new_unit;
        unit_requests[new_unit->id] = Request();
        unit_requests[new_unit->id].type = RequestType::NONE;

        number_of_units[static_cast<std::size_t>(UnitType::RESOURCE)] += 1;
    }
}