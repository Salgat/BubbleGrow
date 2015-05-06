//
// Created by Austin on 5/2/2015.
//

#ifndef BUBBLEGROW_COMMON_HPP
#define BUBBLEGROW_COMMON_HPP

// Standard Library
#include <iostream>
#include <atomic>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <random>

// SFML
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

// Enumerations
enum class ActionType {
    ATTACK,
    GATHER,
    WALK,
    IDLE,
    NONE
};

enum class UnitType {
    BASE,

    // Collector
    GATHERER,

    // Fighting
    FIGHTER,
    BRUTE,

    // Explorer
    SCOUT,

    // Resources
    GROW,
    SUPER_GROW, // A more valuable resource

    NONE
};

enum class AiType {
    EASY,
    NORMAL,
    HARD,
    NONE
};

// Constants
// Todo: May want to set this in a config file

double const PI = 3.141592653589793238463;
unsigned int const RESOLUTION_X = 1280;
unsigned int const RESOLUTION_Y = 1024;
unsigned int const VERSION = 0;
unsigned int const SUB_VERSION = 1;

// Health
int const BASE_MAX_HEALTH = 100;
int const GATHERER_MAX_HEALTH = 100;
int const FIGHTER_MAX_HEALTH = 150;
int const BRUTE_MAX_HEALTH = 200;
int const SCOUT_MAX_HEALTH = 50;

// Walk speed (meters/second)
double const BASE_WALK_SPEED = 20.0;
double const GATHERER_WALK_SPEED = 1.0;
double const FIGHTER_WALK_SPEED = 1.0;
double const BRUTE_WALK_SPEED = 0.7;
double const SCOUT_WALK_SPEED = 1.5;

// Attack speed (attacks/second)
double const BASE_ATTACK_SPEED = 0.5;
double const GATHERER_ATTACK_SPEED = 0.1;
double const FIGHTER_ATTACK_SPEED = 1.0;
double const BRUTE_ATTACK_SPEED = 0.7;
double const SCOUT_ATTACK_SPEED = 1.5;

// Attack damage (health/attack)
int const BASE_ATTACK_DAMAGE = 20;
int const GATHERER_ATTACK_DAMAGE = 1;
int const FIGHTER_ATTACK_DAMAGE = 15;
int const BRUTE_ATTACK_DAMAGE = 35;
int const SCOUT_ATTACK_DAMAGE = 5;

// Attack range (meters from unit position)
double const BASE_ATTACK_RANGE = 1.0;
double const GATHERER_ATTACK_RANGE = 1.0;
double const FIGHTER_ATTACK_RANGE = 1.0;
double const BRUTE_ATTACK_RANGE = 2.0;
double const SCOUT_ATTACK_RANGE = 0.5;

// Unit Size (radius in meters)
double const BASE_SIZE = 1.0;
double const GATHERER_SIZE = 1.0;
double const FIGHTER_SIZE = 1.0;
double const BRUTE_SIZE = 2.0;
double const SCOUT_SIZE = 0.5;

// Resource requirements (resources/unit)
int const BASE_RESOURCE_COST = 10;
int const GATHERER_RESOURCE_COST = 5;
int const FIGHTER_RESOURCE_COST = 5;
int const BRUTE_RESOURCE_COST = 10;
int const SCOUT_RESOURCE_COST = 0;

// Unit conversion requirements


#endif //BUBBLEGROW_COMMON_HPP
