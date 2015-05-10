//
// Created by Austin on 5/5/2015.
//

#ifndef BUBBLEGROW_RESOURCES_HPP
#define BUBBLEGROW_RESOURCES_HPP

#include "Player.hpp"

/**
 * A special neutral Player class implementation to handle resources in the World. Includes manages adding
 * new resources.
 */
class Resources : public Player {
private:

public:
    Resources();

    virtual void Update(double duration);
    virtual void RemoveExpiredUnits();
    void CreateResources(unsigned int amount, float range, float density);
};

#endif //BUBBLEGROW_RESOURCES_HPP
