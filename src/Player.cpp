//
// Created by Austin on 5/3/2015.
//

#include "Player.hpp"
#include "Unit.hpp"
#include "World.hpp"

Player::Player()
    : action_duration(0.0)
    , name("No Name")
    , id(0)
    , army_value(0)
    , resources(0)
    , unit_ids(0)
    , type (PlayerType::PLAYER)
    , ai_type(AiType::NONE)
    , ai_destination(position)
    , wander_range(25.0) {
    // Default to having no units
    for (auto& entry : number_of_units) {
        entry = 0;
    }

    // Default to empty player requests
    for (auto& entry : requests_array) {
        entry = Request();
        entry.type = RequestType::NONE;
    }

    // Choose a random symbol
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 random_generator(seed);
    static std::uniform_real_distribution<> distribution_symbols(0, static_cast<int>(PlayerSymbol::TRIANGLE));
    symbol = static_cast<PlayerSymbol>(distribution_symbols(random_generator));
}

/**
 * Handles player requests in queue.
 */
void Player::ProcessPlayerRequests(double duration) {
    for (auto& request : requests_array) {
        if (request.type == RequestType::PLAYER_WALK) {
            auto destination = sf::Vector2f(request.float_data[0], request.float_data[1]);
            double distance_traveled = duration * kPlayerWalkSpeed * request.float_data[2]; // distance = time * speed

            // First make sure unit isn't already at position
            double x_difference = destination.x - position.x;
            double y_difference = destination.y - position.y;
            auto distance_to =  std::sqrt(x_difference*x_difference + y_difference*y_difference);
            if (distance_to < distance_traveled) {
                request.type = RequestType::NONE;
                continue;
            }

            double angle = atan2(destination.y - position.y, destination.x - position.x);
            position = sf::Vector2f(static_cast<float>(position.x + distance_traveled*cos(angle)),
                                    static_cast<float>(position.y + distance_traveled*sin(angle)));
        } else if (request.type == RequestType::PURCHASE_UNITS) {
            auto purchase_type = static_cast<UnitType>(request.int_data[0]);
            auto purchase_amount = request.int_data[1];
            PurchaseUnits(purchase_amount, purchase_type);
            request.type = RequestType::NONE;
        }
    }
}

/**
 * Updated the walk request for the player.
 */
void Player::PlayerMoveRequest(sf::Vector2f destination, double speed) {
    Request move_request;
    move_request.type = RequestType::PLAYER_WALK;
    move_request.float_data[0] = destination.x;
    move_request.float_data[1] = destination.y;
    move_request.float_data[2] = speed;
    requests_array[static_cast<std::size_t>(RequestType::PLAYER_WALK)] = move_request;
}

void Player::PlayerPurchaseRequest(unsigned int purchase_amount, UnitType purchase_type) {
    Request purchase_request;
    purchase_request.type = RequestType::PURCHASE_UNITS;
    purchase_request.int_data[0] = static_cast<int>(purchase_type);
    purchase_request.int_data[1] = purchase_amount;
    requests_array[static_cast<std::size_t>(RequestType::PURCHASE_UNITS)] = purchase_request;
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
    for (auto& request : unit_requests) {
        auto unit = units.find(request.first)->second;
        if (unit->health[0] > 0)
            unit->ProcessRequest(request.second, duration);
    }
}

/**
 * Process the decisions for each unit.
 */
void Player::MakeDecisions(double duration) {
    for (auto& unit : units) {
        if (unit.second->health[0] > 0)
            unit.second->MakeDecision(unit_requests.find(unit.first)->second);
    }

    MakePlayerDecision(duration);
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
        events.push(Event(EventType::BUBBLE_DIE, units[units_to_remove.top()]->position));
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

    auto unit_resource_cost = kResourceCost[static_cast<int>(type)];
    UnitMainType main_type;
    if (type == UnitType::BASE_LV1 or type == UnitType::BASE_LV2 or type == UnitType::BASE_LV3) {
        main_type = UnitMainType::BASE;
    } else if (type == UnitType::GATHERER_LV1 or type == UnitType::GATHERER_LV2 or type == UnitType::GATHERER_LV3) {
        main_type = UnitMainType::GATHERER;
    } else if (type == UnitType::FIGHTER_LV1 or type == UnitType::FIGHTER_LV2 or type == UnitType::FIGHTER_LV3) {
        main_type = UnitMainType::FIGHTER;
    }

    auto resource_cost = amount * unit_resource_cost;
    if (resource_cost > resources) {
        // Can't afford to create requested amount, create maximum we can afford
        amount = resources / unit_resource_cost;
    }

    number_of_units[static_cast<std::size_t>(type)] += amount;
    while (amount-- > 0) {
        double random_radius = distribution_radius(gen);
        double random_angle = distribution_angle(gen);

        // Initialize a new unit
        resources -= unit_resource_cost;
        auto new_unit = std::make_shared<Unit>();
        new_unit->position = sf::Vector2f(position.x + static_cast<float>(random_radius*std::cos(random_angle)),
                                          position.y + static_cast<float>(random_radius*std::sin(random_angle)));
        new_unit->id = ++unit_ids;
        new_unit->owner_id = id;
        new_unit->owner = shared_from_this();
        new_unit->world = world;
        new_unit->symbol = symbol;
        new_unit->main_type = main_type;
        new_unit->type = type;

        // Add to player's maps
        units[new_unit->id] = new_unit;
        unit_requests[new_unit->id] = Request();
        unit_requests[new_unit->id].type = RequestType::NONE;
    }
}

/**
 * Returns distance (meters) from current unit to target position.
 */
inline double Player::CalculateDistanceTo(sf::Vector2f target_position) {
    double x_difference = target_position.x - position.x;
    double y_difference = target_position.y - position.y;
    return std::sqrt(x_difference*x_difference + y_difference*y_difference);
}

/**
 * Modifies requests with a decision made by the Player's AI.
 */
void Player::MakePlayerDecision(double duration) {
    if (ai_type == AiType::PLAYER) {
        return;
    } else if (ai_type == AiType::EASY) {
        EasyAiDecision(duration);
    } else if (ai_type == AiType::MEDIUM) {
        //MediumAiDecision();
    } else if (ai_type == AiType::HARD) {
        //HardAiDecision();
    } else if (ai_type == AiType::INSANE) {
        //InsaneAiDecision();
    }
}

/**
 * Makes a player decision at the Easy level.
 */
void Player::EasyAiDecision(double duration) {
    // Easy AI simply wanders around randomly and only engages other players if they approach nearby distance
    // Purchase more units if it can afford it
    if (resources > kResourceCost[static_cast<std::size_t>(UnitType::BASE_LV1)]) {
        // Since we just want to spend all the resources, it doesn't matter how many we request (pick an
        // arbitrarily big number)
        PlayerPurchaseRequest(1, UnitType::BASE_LV1);
    }

    // Determine if there are nearby enemy players
    for (auto& player : world->players) {
        if (player.second->type != PlayerType::RESOURCES and player.first != id) {
            double player_distance = CalculateDistanceTo(player.second->position);
            if (player_distance < 3.0 * wander_range) {
                // Nearby player found, head towards him to fight
                PlayerMoveRequest(player.second->position, 1.0);
                return;
            }
        }
    }

    // No nearby players found, so just wander around
    if (CalculateDistanceTo(ai_destination) < 1.0 or action_duration >= kEasyAiIdleTime) {
        ai_destination = MoveTowards(RandomWanderLocation(), 100.0);
        PlayerMoveRequest(ai_destination, 1.0);
        action_duration = 0.0;
    } else {
        action_duration += duration;
    }
}

/**
 * Provides a random location where units can wonder to.
 */
sf::Vector2f Player::RandomWanderLocation() {
    // Choose a random angle (0 to 2*PI) and calculate position based off direction * wander_range
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0, 2*PI);
    double random_angle = dis(gen);
    std::uniform_real_distribution<> dis2(0, world->map_radius);
    double wander_range = dis2(gen);

    return sf::Vector2f(static_cast<float>(wander_range*std::cos(random_angle)),
                        static_cast<float>(wander_range*std::sin(random_angle)));
}

/**
 * Returns the position in the direction of the destination at the provided distance from the current player.
 */
sf::Vector2f Player::MoveTowards(sf::Vector2f destination, double distance) {
    // Find direction, normalize it, multiply it by the scalar (distance), and add to current position
    double x_difference = destination.x - position.x;
    double y_difference = destination.y - position.y;
    double magnitude = std::sqrt(x_difference*x_difference + y_difference*y_difference);
    auto normalized_direction = sf::Vector2f(x_difference/magnitude, y_difference/magnitude);

    return sf::Vector2f(position.x + distance*normalized_direction.x, position.y + distance*normalized_direction.y);
}

/**
 * Creates up to the amount designated of the type specified (either at the cost of resources, units, or both).
 */
void Player::PurchaseUnits(unsigned int amount, UnitType purchase_type) {
    // Todo: There is a bug where it won't buy unless it can afford at least the amount (instead of buying up to the
    // amount)
    // Initialize all costs
    int resource_cost = kResourceCost[static_cast<std::size_t>(purchase_type)];
    UnitType unit_cost_type = kUnitTypeCost[static_cast<std::size_t>(purchase_type)];
    int unit_cost_amount = kUnitCost[static_cast<std::size_t>(purchase_type)];

    // Find the number of units, up to amount, that can be created
    int units_to_purchase = amount;
    auto total_resource_cost = amount * resource_cost;
    if (total_resource_cost > resources) {
        units_to_purchase = resources / resource_cost;
    }

    auto number_of_cost_type = number_of_units[static_cast<std::size_t>(unit_cost_type)];
    auto total_unit_cost = units_to_purchase * unit_cost_amount;
    if (total_unit_cost > number_of_cost_type) {
        units_to_purchase = number_of_cost_type / unit_cost_amount;
    }

    total_resource_cost = units_to_purchase * resource_cost;
    total_unit_cost = units_to_purchase * unit_cost_amount;

    // Remove costs (units and resources) from current player
    //resources.fetch_sub(total_resource_cost);
    number_of_units[static_cast<std::size_t>(unit_cost_type)] -= total_unit_cost;

    std::stack<uint64_t> units_to_remove;
    for (auto& unit : units) {
        if (unit.second->type == unit_cost_type) {
            units_to_remove.push(unit.first);
            --total_unit_cost;
        }

        if (total_unit_cost == 0)
            break;
    }

    while(!units_to_remove.empty()) {
        units.erase(units_to_remove.top());
        unit_requests.erase(units_to_remove.top());
        units_to_remove.pop();
    }

    // Create newly purchased units
    CreateUnits(units_to_purchase, purchase_type);
}