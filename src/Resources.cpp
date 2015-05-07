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
        updated_resources += unit.second->resource_value;
    }
    resources = updated_resources;
}

/**
 * Creates resources on the map randomly distributed from (0.0,0.0) to radius, scaled by density (resources/m^2).
 */
void Resources::CreateResources(unsigned int amount, float radius, float density) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> distribution_radius(0, radius);
    std::uniform_real_distribution<> distribution_angle(0, 2*PI);
    // The resource amount of each unit is inversely proportional to area.
    std::uniform_real_distribution<> distribution_amount(0, amount * density / (radius*radius));

    int current_amount = amount;
    while (current_amount > 0) {
        // Initialize a new resource unit
        double random_radius = distribution_radius(gen);
        double random_angle = distribution_angle(gen);
        unsigned int random_amount = distribution_amount(gen);
        current_amount -= random_amount;

        auto new_unit = std::make_shared<Unit>();
        new_unit->type = UnitType::GROW;
        new_unit->resource_value = random_amount;
        new_unit->size = random_amount/6.0;
        new_unit->position = sf::Vector2f(static_cast<float>(random_radius*std::cos(random_angle)),
                                          static_cast<float>(random_radius*std::sin(random_angle)));
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