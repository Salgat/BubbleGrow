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
#include <chrono>
#include <stack>

//#define NDEBUG
#include <cassert>

// SFML
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

// Enumerations
enum class GameMode {
    MENU,
    IN_GAME,
    NONE
};

enum class MenuType {
    MAIN,
    GAME_MENU,
    BACK,

    // Main submenus
    QUICK_MATCH,
    VERSUS,
    TUTORIAL,
    EXIT_GAME,

    // Versus submenus
    COMPUTER_CHALLENGE,
    MULTI_PLAYER,

    // Game submenu
    LEAVE_GAME,

    NONE
};

// Submenu definitions
std::array<MenuType const, 4> const kMainSubMenus = {MenuType::QUICK_MATCH,
                                                     MenuType::VERSUS,
                                                     MenuType::TUTORIAL,
                                                     MenuType::EXIT_GAME};

std::array<MenuType const, 3> const kVersusSubMenus = {MenuType::COMPUTER_CHALLENGE,
                                                        MenuType::MULTI_PLAYER,
                                                        MenuType::BACK};

std::array<MenuType const, 2> const kGameSubMenus = {MenuType::LEAVE_GAME,
                                                     MenuType::BACK};

// Defines the string representation of the corresponding menu entry
std::array<std::string const, static_cast<std::size_t>(MenuType::NONE)> const kMenuStrings = {"Main Menu",
                                                                                              "Game Menu",
                                                                                              "Back",
                                                                                              "Quick Match",
                                                                                              "Versus Mode",
                                                                                              "Tutorial",
                                                                                              "Exit Game",
                                                                                              "Computer Challenge",
                                                                                              "Multiplayer Mode",
                                                                                              "Leave Game"};

// Standard Font Sizes
enum TextSize {
    GAME_MENU = 144,
    RESOURCE_COUNTER = 24,

    MAIN = 144,
    VERSUS = 144,


    NONE
};

// Image IDs

enum class ImageId {
    LOGO,
    BUBBLE,
    BASE_LV1,
    BASE_LV2,
    BASE_LV3,
    GATHERER_LV1,
    GATHERER_LV2,
    GATHERER_LV3,
    FIGHTER_LV1,
    FIGHTER_LV2,
    FIGHTER_LV3,
    ARROW,
    NONE
};

enum class ActionType {
    ATTACK,
    GATHER,
    WALK,
    IDLE,
    NONE
};


enum class UnitType {
    BASE_LV1,
    BASE_LV2,
    BASE_LV3,

    // Collector
    GATHERER_LV1,
    GATHERER_LV2,
    GATHERER_LV3,

    // Fighting
    FIGHTER_LV1,
    FIGHTER_LV2,
    FIGHTER_LV3,

    // Explorer
    SCOUT,

    // Resources
    RESOURCE,
    SUPER_RESOURCE, // A more valuable resource

    NONE
};

// A less specific classifier for units based on their role
enum class UnitMainType {
    BASE,
    GATHERER,
    FIGHTER,
    RESOURCE,
    NONE
};

enum class AiType {
    PLAYER, // Player AI means just skipping the process of making a decision (and leaves that up to the user input)
    EASY,
    MEDIUM,
    HARD,
    INSANE, // Insane difficulty is allowed to cheat
    NONE
};

double const kEasyAiIdleTime = 6.0;

enum class PlayerType {
    PLAYER,
    COMPUTER,
    RESOURCES,
    NONE
};

// Constants
// Todo: May want to set this in a config file
double const kPlayerWalkSpeed = 20.0;

double const PI = 3.141592653589793238463;
unsigned int const RESOLUTION_X = 1280;
unsigned int const RESOLUTION_Y = 1024;
unsigned int const VERSION = 0;
unsigned int const SUB_VERSION = 1;

// Health
int const kBaseMaxHealth = 100;
int const kGathererMaxHealth = 100;
int const kFighterMaxHealth = 150;
int const kBruteMaxHealth = 200;
int const kScoutMaxHealth = 50;

// Walk speed (meters/second)
double const kBaseWalkSpeed = 20.0;
double const kGathererWalkSpeed = 1.0;
double const kFighterWalkSpeed = 1.0;
double const kBruteWalkSpeed = 0.7;
double const kScoutWalkSpeed = 1.5;

// Attack speed (attacks/second)
double const kBaseAttackSpeed = 0.5;
double const kGathererAttackSpeed = 0.1;
double const kFighterAttackSpeed = 1.0;
double const kBruteAttackSpeed = 0.7;
double const kScoutAttackSpeed = 1.5;

// Attack damage (health/attack)
int const kBaseAttackDamage = 20;
int const kGathererAttackDamage = 1;
int const kFighterAttackDamage = 15;
int const kBruteAttackDamage = 35;
int const kScoutAttackDamage = 5;

// Attack range (meters from unit position)
double const kBaseAttackRange = 1.0;
double const kGathererAttackRange = 1.0;
double const kFighterAttackRange = 1.0;
double const kBruteAttackRange = 2.0;
double const kScoutAttackRange = 0.5;

// Unit Size (radius in meters)
double const kBaseSize = 1.0;
double const kGathererSize = 1.0;
double const kFighterSize = 1.0;
double const kBruteSize = 2.0;
double const kScoutSize = 0.5;

// Gather speed (gathers/second)
double const kBaseGatherSpeed = 0.2;
double const kGathererGatherSpeed = 1.0;
double const kFighterGatherSpeed = 0.1;
double const kBruteGatherSpeed = 0.1;
double const kScoutGatherSpeed = 0.2;

// Gather amount (resources/gather)
int const kBaseGatherAmount = 1;
int const kGathererGatherAmount = 1;
int const kFighterGatherAmount = 1;
int const kBruteGatherAmount = 1;
int const kScoutGatherAmount = 1;

// Constant Arrays for values. It's important that the order of the entries match the order of the entries in UnitType.
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kMaxHealth = {kBaseMaxHealth,
                                                                                    kGathererMaxHealth,
                                                                                    kFighterMaxHealth,
                                                                                    kBruteMaxHealth,
                                                                                    kScoutMaxHealth};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kWalkSpeed = {kBaseWalkSpeed,
                                                                                       kGathererWalkSpeed,
                                                                                       kFighterWalkSpeed,
                                                                                       kBruteWalkSpeed,
                                                                                       kScoutWalkSpeed};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kAttackSpeed = {kBaseAttackSpeed,
                                                                                         kGathererAttackSpeed,
                                                                                         kFighterAttackSpeed,
                                                                                         kBruteAttackSpeed,
                                                                                         kScoutAttackSpeed};

std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kAttackDamage = {kBaseAttackDamage,
                                                                                       kGathererAttackDamage,
                                                                                       kFighterAttackDamage,
                                                                                       kBruteAttackDamage,
                                                                                       kScoutAttackDamage};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kAttackRange = {kBaseAttackRange,
                                                                                         kGathererAttackRange,
                                                                                         kFighterAttackRange,
                                                                                         kBruteAttackRange,
                                                                                         kScoutAttackRange};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kSize = {kBaseSize,
                                                                                  kGathererSize,
                                                                                  kFighterSize,
                                                                                  kBruteSize,
                                                                                  kScoutSize};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kGatherSpeed = {kBaseGatherSpeed,
                                                                                         kGathererGatherSpeed,
                                                                                         kFighterGatherSpeed,
                                                                                         kBruteGatherSpeed,
                                                                                         kScoutGatherSpeed};

std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kGatherAmount = {kBaseGatherAmount,
                                                                                       kGathererGatherAmount,
                                                                                       kFighterGatherAmount,
                                                                                       kBruteGatherAmount,
                                                                                       kScoutGatherAmount};

// Unit cost requirements
// Resource requirements (resources/unit)
int const kBaseResourceCost = 100;
int const kGathererResourceCost = 0;
int const kFighterResourceCost = 0;
int const kBruteResourceCost = 0;
int const kScoutResourceCost = 0;

// Unit Type cost
UnitType const kBaseUnitTypeCost = UnitType::NONE;
UnitType const kGathererUnitTypeCost = UnitType::BASE_LV1;
UnitType const kFighterUnitTypeCost = UnitType::BASE_LV1;
UnitType const kBruteUnitTypeCost = UnitType::FIGHTER_LV1;
UnitType const kScoutUnitTypeCost = UnitType::BASE_LV1;

// Unit cost requirements (units/new unit)
int const kBaseUnitCost = 0;
int const kGathererUnitCost = 5;
int const kFighterUnitCost = 5;
int const kBruteUnitCost = 5;
int const kScoutUnitCost = 5;

// Determines cost in resources of the unit type
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kResourceCost = {kBaseResourceCost,
                                                                                       kGathererResourceCost,
                                                                                       kFighterResourceCost,
                                                                                       kBruteResourceCost,
                                                                                       kScoutResourceCost};

// Determines the type of unit for "unit costs"
std::array<UnitType const, static_cast<std::size_t>(UnitType::NONE)> const kUnitTypeCost = {kBaseUnitTypeCost,
                                                                                       kGathererUnitTypeCost,
                                                                                       kFighterUnitTypeCost,
                                                                                       kBruteUnitTypeCost,
                                                                                       kScoutUnitTypeCost};

// Determines the cost in units
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kUnitCost = {kBaseUnitCost,
                                                                                   kGathererUnitCost,
                                                                                   kFighterUnitCost,
                                                                                   kBruteUnitCost,
                                                                                   kScoutUnitCost};

// Tiles to use
// Defines the string representation of the corresponding menu entry
std::array<std::string const, 1> const kTileStrings = {"BG_Tile1.png"};
std::size_t const tilemap_dimensions = 20; // 20x20 background tile map
double const parallax_strength = 10.0; // Determines how fast the background moves in relation to player position

#endif //BUBBLEGROW_COMMON_HPP

/**
 * Note: Use https://google-styleguide.googlecode.com/svn/trunk/cppguide.html (Google C++) style guide.
 */