//
// Created by Austin on 5/2/2015.
//

#ifndef BUBBLEGROW_REQUEST_HPP
#define BUBBLEGROW_REQUEST_HPP

#include "common.hpp"

enum class RequestType {
    // Unit requests
    WALK,
    ATTACK,
    GATHER,
    CAST,
    IDLE,

    // Player requests
    CREATE_BASE_UNITS, // Create base units using resources
    CONVERT_UNITS, // Converts units to desired type

    NONE
};

std::size_t const REQUEST_ARRAY_SIZE = 10;

/**
 *
 */
struct Request {
    RequestType type;
    std::array<int, REQUEST_ARRAY_SIZE> int_data;
    std::array<float, REQUEST_ARRAY_SIZE> float_data;
};

#endif //BUBBLEGROW_REQUEST_HPP

/**
 * Request formats:
 *  WALK:
 *   - type = RequestType::WALK
 *   - float_data[0] = x coordinate destination
 *   - float_data[1] = y coordinate destination
 *
 *  ATTACK:
 *   - type = RequestType::ATTACK
 *   - int_data[0] = target owner id
 *   - int_data[1] = target id
 *
 *  GATHER:
 *   - type = RequestType::GATHER
 *   - int_data[0] = target owner id
 *   - int_data[1] = target id
 */