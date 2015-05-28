//
// Created by Austin on 5/3/2015.
//

#ifndef BUBBLEGROW_PLAYER_HPP
#define BUBBLEGROW_PLAYER_HPP

#include "common.hpp"
#include "Request.hpp"
#include "Event.hpp"

class Unit;
class World;

class Player : public std::enable_shared_from_this<Player> {
private:
    inline double CalculateDistanceTo(sf::Vector2f target_position);
    inline sf::Vector2f MoveTowards(sf::Vector2f destination, double distance);

    // Player AI
    // Todo: Consider encapsulating the AI state and logic into a separate class
    double action_duration;
    sf::Vector2f ai_destination;

    void MakePlayerDecision(double duration);
    void EasyAiDecision(double duration);

public:
    // Player information
    std::string name; // Visible name to all players
    uint64_t id;
    uint64_t unit_ids; // Increment and assign each time a new unit is created
    PlayerType type;
    AiType ai_type;

    std::shared_ptr<World> world;

    std::stack<Event> events;

    std::queue<Request> requests; // Requests for the player class to process (typically based on input from player)
    std::array<Request, static_cast<std::size_t>(RequestType::PLAYER_END_REQUESTS)> requests_array;

    /* These containers are special, since they will be accessed concurrently. The rule is, when a new unit is created,
   a new entry is added for this unit (in a thread safe area). We can concurrently access this map
   as long as we aren't creating or removing entries and aren't writing to the same entry from two different
   threads. As an added note, only const functions will be guaranteed not to modify the structure; this means that
   we cannot use operator[], but can use iterator functions including unordered_map::find.

   Note: Yes, using a concurrent library like TBB would be ideal (I wish!), but we want to keep external library
   dependencies to a minimum! */
    std::unordered_map<uint64_t, std::shared_ptr<Unit>> units; // A map of all units (key = id) owned by player
    std::unordered_map<uint64_t, Request> unit_requests;

    // Player stats
    sf::Vector2f position;
    std::array<unsigned int, static_cast<std::size_t>(UnitType::NONE)> number_of_units;
    unsigned int army_value; // The "score" based on number of units and their types
    std::atomic<int> resources; // Used to create more base units
    double wander_range; // Radius in meters that units will wander

    // Render details
    sf::Color color;
    PlayerSymbol symbol;

    Player();

    void ProcessPlayerRequests(double duration);
    void PlayerMoveRequest(sf::Vector2f destination, double speed);
    void PlayerPurchaseRequest(unsigned int purchase_amount, UnitType purchase_type);

    virtual void Update(double duration);
    void ProcessRequests(double duration); // Duration is in seconds (example: 0.01666 == 60fps)
    void MakeDecisions(double duration);
    virtual void RemoveExpiredUnits();

    void CreateUnits(int amount, UnitType type);

    sf::Vector2f RandomWanderLocation();

    void PurchaseUnits(unsigned int amount, UnitType purchase_type);
};

#endif //BUBBLEGROW_PLAYER_HPP
