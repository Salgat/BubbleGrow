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

    // Default to empty player requests
    for (auto& entry : requests_array) {
        entry = Request();
        entry.type = RequestType::NONE;
    }
}

/**
 * Handles player requests in queue.
 */
void Player::ProcessPlayerRequests(double duration) {
    for (auto& request : requests_array) {
        if (request.type == RequestType::PLAYER_WALK) {
            auto destination = sf::Vector2f(request.float_data[0], request.float_data[1]);
            double distance_traveled = duration * kPlayerWalkSpeed; // distance = time * speed

            // First make sure unit isn't already at position
            double x_difference = destination.x - position.x;
            double y_difference = destination.y - position.y;
            auto distance_to =  std::sqrt(x_difference*x_difference + y_difference*y_difference);
            if (distance_to < distance_traveled) {
                request.type = RequestType::NONE;
                return;
            }

            double angle = atan2(destination.y - position.y, destination.x - position.x);
            position = sf::Vector2f(static_cast<float>(position.x + distance_traveled*cos(angle)),
                                    static_cast<float>(position.y + distance_traveled*sin(angle)));
        }
    }
}

/**
 * Handles player requests in queue.
 */
/*
void Player::ProcessPlayerRequests(double duration) {
    std::map<RequestType, bool> request_types_processed;
    std::queue<Request> renewed_requests;
    while (!requests.empty()) {
        auto request = requests.front();

        if (request.type == RequestType::PLAYER_WALK and !request_types_processed.count(RequestType::PLAYER_WALK)) {
            auto destination = sf::Vector2f(request.float_data[0], request.float_data[1]);
            double distance_traveled = duration * kPlayerWalkSpeed; // distance = time * speed

            // First make sure unit isn't already at position
            double x_difference = destination.x - position.x;
            double y_difference = destination.y - position.y;
            auto distance_to =  std::sqrt(x_difference*x_difference + y_difference*y_difference);
            if (distance_to < distance_traveled) {
                requests.pop();
                return;
            }

            double angle = atan2(destination.y - position.y, destination.x - position.x);
            position = sf::Vector2f(static_cast<float>(position.x + distance_traveled*cos(angle)),
                                    static_cast<float>(position.y + distance_traveled*sin(angle)));
            renewed_requests.push(request);
        }

        requests.pop();
    }

    requests = renewed_requests;
}*/

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
