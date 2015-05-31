//
// Created by Austin on 5/2/2015.
//

#ifndef BUBBLEGROW_UNIT_HPP
#define BUBBLEGROW_UNIT_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Event.hpp"

class Player;
class World;

class Unit {
private:
    inline double CalculateWalkSpeed();
    inline double CalculateAttackSpeed();
    inline int CalculateAttackDamage();
    inline double CalculateAttackRange();
    inline double CalculateGatherSpeed();
    inline int CalculateGatherAmount();

    inline double CalculateDistanceTo(sf::Vector2f target_position, bool use_owner_position = false);

    inline sf::Vector2f RandomWanderLocation();

    std::shared_ptr<Unit> FindClosestEnemy();
    std::shared_ptr<Unit> FindClosestResource();
    std::shared_ptr<Unit> FindClosestUnit(PlayerType ignore = PlayerType::NONE);

    void WalkTo(sf::Vector2f destination, double duration, bool update_action);
    void Attack(uint64_t target, uint64_t target_owner, double duration);
    void Gather(uint64_t target, uint64_t target_owner, double duration);

public:
    uint64_t id; // Each id/owner_id pair is assumed to be unique, in that no two units will ever have the same pair
    uint64_t owner_id;
    std::shared_ptr<Player> owner; // Used to gain knowledge like current location of player (owner)
    std::shared_ptr<World> world;

    UnitMainType main_type;
    UnitType type;
    PlayerSymbol symbol;
    sf::Vector2f position;
    float size; // Radius of unit (meters)
    std::array<std::atomic<int>, 2> health;
    double walk_speed;
    double attack_speed;
    int attack_damage;

    ActionType action;
    double action_duration;
    uint64_t action_target;

    // Update information
    std::atomic<int> pending_damage; // Damage to apply to unit from other sources
    std::atomic<int> resource_value; // Value if the unit is a resource

    std::stack<Event> events;

    Unit();
    void Update(double duration);
    void MakeDecision(Request& request);
    void ProcessRequest(Request& request, double duration);


};

#endif //BUBBLEGROW_UNIT_HPP
