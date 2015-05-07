//
// Created by Austin on 5/2/2015.
//

#include "Unit.hpp"
#include "Player.hpp"
#include "World.hpp"

/**
 * Creates a default base type unit. It's important to remember that this only creates a base unit with no owner, and
 * the player class needs to finish setting the attributes for this new unit.
 */
Unit::Unit()
    : id(0)
    , owner_id(0)
    , type(UnitType::BASE)
    , position(sf::Vector2f(0.0, 0.0))
    , size(1.0)
    , walk_speed(BASE_WALK_SPEED)
    , attack_speed(BASE_ATTACK_SPEED)
    , attack_damage(BASE_ATTACK_DAMAGE)
    , action(ActionType::NONE)
    , pending_damage(0.0) {
    health[0] = BASE_MAX_HEALTH;
    health[1] = BASE_MAX_HEALTH;
}

/**
 * Updates unit's stats for next cycle.
 */
void Unit::Update(double duration) {
    health[0] += pending_damage.load();
}

/**
 * Modifies the unit based on the request argument.
 */
void Unit::ProcessRequest(Request& request, double duration) {
    if (request.type == RequestType::IDLE) {
        // Stops the unit and has him do nothing
        if (action == ActionType::WALK or action == ActionType::NONE) {
            action = ActionType::IDLE;
            request.type = RequestType::NONE;
        }
    } else if (request.type == RequestType::WALK) {
        // Commands a unit to start walking in the direction provided
        if (action == ActionType::WALK or action == ActionType::IDLE or action == ActionType::NONE) {
            WalkTo(sf::Vector2f(request.float_data[0], request.float_data[1]), duration, true);
        }
    } else if (request.type == RequestType::ATTACK) {
        // Commands unit to attack a unit if within attack range, if not within range will instead walk towards unit
        if (action == ActionType::WALK or action == ActionType::IDLE or action == ActionType::NONE or
            action == ActionType::ATTACK) {
            Attack(static_cast<uint64_t>(request.int_data[1]), static_cast<uint64_t>(request.int_data[0]), duration);
        }
    }
}

/**
 * Returns a request made by the unit's AI.
 *
 * Todo: Calculating closest_enemy and closest_resource before checking unit type is inefficient.
 */
void Unit::MakeDecision(Request& request) {
    // Attacking is prioritized
    auto closest_enemy = FindClosestEnemy();
    //auto closest_enemy = FindClosestEnemy();
    if (closest_enemy and (type == UnitType::BASE or type == UnitType::FIGHTER or type == UnitType::BRUTE)) {
        request.type = RequestType::ATTACK;
        request.int_data[0] = static_cast<int>(closest_enemy->owner_id);
        request.int_data[1] = static_cast<int>(closest_enemy->id);
    } else {
        // No enemies, try to find resource to gather
        auto closest_resource = FindClosestResource();
        if (closest_resource and (type == UnitType::BASE or type == UnitType::GATHERER)) {
            request.type = RequestType::GATHER;
            request.int_data[0] = static_cast<int>(closest_resource->owner_id);
            request.int_data[1] = static_cast<int>(closest_resource->id);
        } else if (action != ActionType::WALK) {
            // No enemies to attack and no resources to gather, so just wander around
            auto wander_position = RandomWanderLocation();
            request.type = RequestType::WALK;
            request.float_data[0] = wander_position.x;
            request.float_data[1] = wander_position.y;
        }
    }
}

/**
 * Helper function that returns walk speed base on unit's type.
 */
double Unit::CalculateWalkSpeed() {
    if (type == UnitType::BASE) {
        return BASE_WALK_SPEED;
    }

    return 0.0;
}

double Unit::CalculateAttackSpeed() {
    if (type == UnitType::BASE) {
        return BASE_ATTACK_SPEED;
    }

    return 0.0;
}

int Unit::CalculateAttackDamage() {
    if (type == UnitType::BASE) {
        return BASE_ATTACK_DAMAGE;
    }

    return 0;
}

inline double Unit::CalculateAttackRange() {
    if (type == UnitType::BASE) {
        return BASE_ATTACK_RANGE;
    }

    return 0.0;
}

inline double Unit::CalculateDistanceTo(sf::Vector2f target_position) {
    double x_difference = target_position.x - position.x;
    double y_difference = target_position.y - position.y;
    return std::sqrt(x_difference*x_difference + y_difference*y_difference);
}

/**
 * Provides a random location where units can wonder to.
 */
inline sf::Vector2f Unit::RandomWanderLocation() {
    // Choose a random angle (0 to 2*PI) and calculate position based off direction * wander_range
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0, 2*PI);
    double random_angle = dis(gen);
    std::uniform_real_distribution<> dis2(0, owner->wander_range);
    double wander_range = dis2(gen);

    return sf::Vector2f(static_cast<float>(wander_range*std::cos(random_angle)),
                        static_cast<float>(wander_range*std::sin(random_angle)));
}

/**
 * Helper function to "walk" the unit towards destination.
 */
void Unit::WalkTo(sf::Vector2f destination, double duration, bool update_action) {
    double distance_traveled = duration * CalculateWalkSpeed(); // distance = time * speed

    // First make sure unit isn't already at position
    if (CalculateDistanceTo(destination) < distance_traveled) {
        if (update_action) action = ActionType::IDLE;
        return;
    }

    double angle = atan2(destination.y - position.y, destination.x - position.x);
    position = sf::Vector2f(static_cast<float>(position.x + distance_traveled*cos(angle)),
                            static_cast<float>(position.y + distance_traveled*sin(angle)));
    if (update_action) action = ActionType::WALK;
}

/**
 * Helper function to process an attack (either a new attack, or an ongoing attack).
 */
void Unit::Attack(uint64_t target, uint64_t target_owner, double duration) {
    auto target_unit = world->FindUnit(target_owner, target);

    if (action == ActionType::ATTACK) {
        // Complete current attack (regardless of what the new target is)
        if (action_duration >= CalculateAttackSpeed()) {
            // Attack has finished
            if (target_unit)
                target_unit->health[0].fetch_add(-1 * CalculateAttackDamage());
            action = ActionType::IDLE;
        } else {
            action_duration += duration;
        }
    } else if (target_unit) {
        auto target_position = target_unit->position;
        auto range = CalculateDistanceTo(target_position);
        if (CalculateAttackRange() >= range) {
            // Start a new attack
            action = ActionType::ATTACK;
            action_target = target;
            action_duration = 0.0;

            Attack(target, target_owner,duration);
        } else {
            // Outside attack range, walk towards target
            WalkTo(target_position, duration, false);
        }
    } else {
        // No enemy, set action back to idle
        action = ActionType::IDLE;
    }
}

/**
 * Returns shared_ptr to closest unit, otherwise returns nullptr. Ignores players of "PlayerType ignore". Mainly used
 * as a helper class.
 */
std::shared_ptr<Unit> Unit::FindClosestUnit(PlayerType ignore) {
    std::pair<double, std::shared_ptr<Unit>> closest_unit = std::make_pair(std::numeric_limits<double>::max(), nullptr);
    for (auto& player : world->players) {
        if (player.first != owner_id and player.second->type != ignore) {
            for (auto& unit : player.second->units) {
                int required_amount = 0;
                if (ignore == PlayerType::RESOURCES)
                    required_amount = unit.second->health[0];
                else
                    return nullptr; // Todo: Implement resources still
                    //required_amount = unit.second->resource_value;

                if (required_amount > 0) {
                    auto distance = CalculateDistanceTo(unit.second->position);
                    if (distance < closest_unit.first) {
                        closest_unit = std::make_pair(distance, unit.second);
                    }
                }
            }
        }
    }

    return closest_unit.second;
}

/**
 * Returns shared_ptr to closest enemy in world.
 */
std::shared_ptr<Unit> Unit::FindClosestEnemy() {
    return FindClosestUnit(PlayerType::RESOURCES);
}

/**
 * Returns shared_ptr to closest resource in world.
 */
std::shared_ptr<Unit> Unit::FindClosestResource() {
    return FindClosestUnit(PlayerType::NONE);
}