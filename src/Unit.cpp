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
            WalkTo(sf::Vector2f(request.float_data[0], request.float_data[1]), duration);
        }
    } else if (request.type == RequestType::ATTACK) {
        // Commands unit to attack a unit if within attack range, if not within range will instead walk towards unit
        if (action == ActionType::WALK or action == ActionType::IDLE or action == ActionType::NONE or
            action == ActionType::ATTACK) {
            Attack(static_cast<uint64_t>(request.int_data[0]), duration);
        }
    }
}

/**
 * Returns a request made by the unit's AI.
 *
 * Todo: May be more efficient to pass in a reference of a Request to modify.
 * Todo 2: Calculating closest_enemy and closest_resource before checking unit type is inefficient.
 */
Request Unit::MakeDecision() {
    auto request = Request();
    request.type = RequestType::NONE;

    // Attacking is prioritized
    auto closest_enemy = FindClosestEnemy();
    if (closest_enemy and (type == UnitType::BASE or type == UnitType::FIGHTER or type == UnitType::BRUTE)) {
        request.type = RequestType::ATTACK;
        request.int_data[0] = static_cast<int>(closest_enemy->id);
    } else {
        // No enemies, try to find resource to gather
        auto closest_resource = FindClosestResource();
        if (closest_resource and (type == UnitType::BASE or type == UnitType::GATHERER)) {
            request.type = RequestType::GATHER;
            request.int_data[0] = static_cast<int>(closest_resource->id);
        } else {
            // No enemies to attack and no resources to gather, so just wander around
            auto wander_position = RandomWanderLocation();
            request.type = RequestType::WALK;
            request.float_data[0] = wander_position.x;
            request.float_data[1] = wander_position.y;
        }
    }

    return request;
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 2*PI);
    double random_angle = dis(gen);
    double wander_range = owner->wander_range;

    return sf::Vector2f(static_cast<float>(wander_range*std::cos(random_angle)),
                        static_cast<float>(wander_range*std::sin(random_angle)));
}

/**
 * Helper function to "walk" the unit towards destination.
 */
void Unit::WalkTo(sf::Vector2f destination, double duration) {
    double distance_traveled = duration * CalculateWalkSpeed(); // distance = time * speed
    double angle = atan2(destination.y - position.y, destination.x - position.x);
    position = sf::Vector2f(static_cast<float>(position.x + distance_traveled*cos(angle)),
                            static_cast<float>(position.y + distance_traveled*sin(angle)));
    action = ActionType::WALK;
}

/**
 * Helper function to process an attack (either a new attack, or an ongoing attack).
 */
void Unit::Attack(uint64_t target, double duration) {
    auto target_unit = world->FindUnit(target);

    if (action == ActionType::ATTACK) {
        // Complete current attack (regardless of what the new target is)
        if (action_duration >= CalculateAttackSpeed()) {
            // Attack has finished
            if (target_unit)
                target_unit->pending_damage.fetch_add(CalculateAttackDamage());
            action = ActionType::IDLE;
        } else {
            action_duration += duration;
        }
    } else if (target_unit) {
        auto target_position = target_unit->position;
        auto range = CalculateDistanceTo(target_position);
        if (CalculateAttackRange() <= range) {
            // Start a new attack
            action = ActionType::ATTACK;
            action_target = target;
            action_duration = 0.0;

            Attack(target, duration);
        } else {
            // Outside attack range, walk towards target
            WalkTo(target_position, duration);
        }
    }
}

/**
 * Returns shared_ptr to closest enemy, otherwise returns nullptr.
 */
std::shared_ptr<Unit> Unit::FindClosestEnemy() {

    return nullptr;
}

/**
 * Returns shared_ptr to closest resource, otherwise returns nullptr.
 */
std::shared_ptr<Unit> Unit::FindClosestResource() {

    return nullptr;
}