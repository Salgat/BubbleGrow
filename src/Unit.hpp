//
// Created by Austin on 5/2/2015.
//

#ifndef BUBBLEGROW_UNIT_HPP
#define BUBBLEGROW_UNIT_HPP

#include "common.hpp"
#include "Request.hpp"

class Player;
class World;

class Unit {
private:
    inline double CalculateWalkSpeed();
    inline double CalculateAttackSpeed();
    inline int CalculateAttackDamage();
    inline double CalculateAttackRange();
    inline double CalculateDistanceTo(sf::Vector2f target_position);

    inline sf::Vector2f RandomWanderLocation();

    std::shared_ptr<Unit> FindClosestEnemy();
    std::shared_ptr<Unit> FindClosestResource();

    void WalkTo(sf::Vector2f destination, double duration);
    void Attack(uint64_t target, uint64_t target_owner, double duration);

public:
    uint64_t id; // Each id/owner_id pair is assumed to be unique, in that no two units will ever have the same pair
    uint64_t owner_id;
    std::shared_ptr<Player> owner; // Used to gain knowledge like current location of player (owner)
    std::shared_ptr<World> world;

    UnitType type;
    sf::Vector2f position;
    std::array<int, 2> health;
    double walk_speed;
    double attack_speed;
    int attack_damage;

    ActionType action;
    double action_duration;
    uint64_t action_target;

    // Update information
    std::atomic<int> pending_damage; // Damage to apply to unit from other sources

    Unit();
    void Update(double duration);
    Request MakeDecision();
    void ProcessRequest(Request& request, double duration);


};

#endif //BUBBLEGROW_UNIT_HPP
