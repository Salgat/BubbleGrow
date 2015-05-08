//
// Created by Austin on 5/2/2015.
//

#ifndef BUBBLEGROW_REQUEST_HPP
#define BUBBLEGROW_REQUEST_HPP

#include "common.hpp"

enum class RequestType {
    // Player requests
    PLAYER_WALK,
    CREATE_BASE_UNITS, // Create base units using resources
    CONVERT_UNITS, // Converts units to desired type
    PLAYER_END_REQUESTS,

    // Unit requests
    WALK,
    ATTACK,
    GATHER,
    CAST,
    IDLE,

    NONE
};

std::size_t const REQUEST_ARRAY_SIZE = 2; // Has to be large enough to hold all request formats.

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
 * Units,
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
 *
 * Players,
 *   PLAYER_WALK:
 *    - type = RequestType::PLAYER_WALK
 *   - float_data[0] = x coordinate destination
 *   - float_data[1] = y coordinate destination
 */