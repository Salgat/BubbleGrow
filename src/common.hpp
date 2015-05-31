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
#include <tuple>
#include <functional>

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
    WARNING = 100,
    RESOURCE_COUNTER = 32,

    MAIN = 144,
    VERSUS = 144,
};

// Image IDs

enum ImageId {
    // Indexes for each of the Bubble Unit types: BASE_LV1 = (0, 0), FIGHTER_LV2 = (std::floor(7/3), 7%3)
    BASE_LV1 = 0,
    BASE_LV2 = 1,
    BASE_LV3 = 2,
    GATHERER_LV1 = 3,
    GATHERER_LV2 = 4,
    GATHERER_LV3 = 5,
    FIGHTER_LV1 = 6,
    FIGHTER_LV2 = 7,
    FIGHTER_LV3 = 8,



    LOGO,
    HOTKEY_BAR,
    BUBBLE,
    BUBBLE_TYPES, // Sprite sheet for Bubble Units
    PLAYER_SYMBOLS, // Sprite sheet for Player Symbols

    ARROW
};

// Identifiers for every sound in the game
enum class SoundId {
    BUBBLE_DIE,
    BUBBLE_ATTACK,
    BUBBLE_GATHER,
    MENU_SELECTION,
    MENU_SELECTION_CHANGE,
    MAIN_MENU_MUSIC,
    IN_GAME_MUSIC,
    NONE
};
//std::array<std::string const, static_cast<std::size_t>(SoundId::NONE)> const kSoundIdFiles = {};

// Indexes for each of the Player symbols (in the spritesheet)
enum PlayerSymbol {
    CIRCLE = 0,
    CRESCENT = 1,
    DIAMOND = 2,
    EYE = 3,
    SHAMROCK = 4,
    SQUARE = 5,
    STAR = 6,
    TRIANGLE = 7,
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
extern unsigned int ResolutionX;
extern unsigned int ResolutionY;
unsigned int const VERSION = 0;
unsigned int const SUB_VERSION = 1;

// Health
int const kBaseMaxHealth = 100;
int const kBase2MaxHealth = 100;
int const kBase3MaxHealth = 100;
int const kGathererMaxHealth = 100;
int const kGatherer2MaxHealth = 100;
int const kGatherer3MaxHealth = 100;
int const kFighterMaxHealth = 150;
int const kFighter2MaxHealth = 200;
int const kFighter3MaxHealth = 200;

// Walk speed (meters/second)
double const kBaseWalkSpeed = 20.0;
double const kBase2WalkSpeed = 20.0;
double const kBase3WalkSpeed = 20.0;
double const kGathererWalkSpeed = 20.0;
double const kGatherer2WalkSpeed = 20.0;
double const kGatherer3WalkSpeed = 20.0;
double const kFighterWalkSpeed = 20.0;
double const kFighter2WalkSpeed = 20.0;
double const kFighter3WalkSpeed = 20.0;

// Attack speed (seconds/attack)
double const kBaseAttackSpeed = 1.0;
double const kBase2AttackSpeed = 1.0;
double const kBase3AttackSpeed = 1.0;
double const kGathererAttackSpeed = 0.1;
double const kGatherer2AttackSpeed = 0.1;
double const kGatherer3AttackSpeed = 0.1;
double const kFighterAttackSpeed = 1.0;
double const kFighter2AttackSpeed = 0.7;
double const kFighter3AttackSpeed = 0.7;

// Attack damage (health/attack)
int const kBaseAttackDamage = 25;
int const kBase2AttackDamage = 25;
int const kBase3AttackDamage = 25;
int const kGathererAttackDamage = 1;
int const kGatherer2AttackDamage = 1;
int const kGatherer3AttackDamage = 1;
int const kFighterAttackDamage = 15;
int const kFighter2AttackDamage = 35;
int const kFighter3AttackDamage = 35;

// Attack range (meters from unit position)
double const kBaseAttackRange = 1.0;
double const kBase2AttackRange = 1.0;
double const kBase3AttackRange = 1.0;
double const kGathererAttackRange = 1.0;
double const kGatherer2AttackRange = 1.0;
double const kGatherer3AttackRange = 1.0;
double const kFighterAttackRange = 1.0;
double const kFighter2AttackRange = 2.0;
double const kFighter3AttackRange = 2.0;

// Unit Size (radius in meters)
double const kBaseSize = 1.0;
double const kBase2Size = 1.0;
double const kBase3Size = 1.0;
double const kGathererSize = 1.0;
double const kGatherer2Size = 1.0;
double const kGatherer3Size = 1.0;
double const kFighterSize = 1.0;
double const kFighter2Size = 2.0;
double const kFighter3Size = 8.0;

// Gather speed (gathers/second)
double const kBaseGatherSpeed = 0.2;
double const kBase2GatherSpeed = 0.2;
double const kBase3GatherSpeed = 0.2;
double const kGathererGatherSpeed = 1.0;
double const kGatherer2GatherSpeed = 1.0;
double const kGatherer3GatherSpeed = 1.0;
double const kFighterGatherSpeed = 0.1;
double const kFighter2GatherSpeed = 0.1;
double const kFighter3GatherSpeed = 0.1;

// Gather amount (resources/gather)
int const kBaseGatherAmount = 1;
int const kBase2GatherAmount = 1;
int const kBase3GatherAmount = 1;
int const kGathererGatherAmount = 1;
int const kGatherer2GatherAmount = 1;
int const kGatherer3GatherAmount = 1;
int const kFighterGatherAmount = 1;
int const kFighter2GatherAmount = 1;
int const kFighter3GatherAmount = 1;

// Constant Arrays for values. It's important that the order of the entries match the order of the entries in UnitType.
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kMaxHealth = {kBaseMaxHealth,
                                                                                    kBase2MaxHealth,
                                                                                    kBase3MaxHealth,
                                                                                    kGathererMaxHealth,
                                                                                    kGatherer2MaxHealth,
                                                                                    kGatherer3MaxHealth,
                                                                                    kFighterMaxHealth,
                                                                                    kFighter2MaxHealth,
                                                                                    kFighter3MaxHealth};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kWalkSpeed = {kBaseWalkSpeed,
                                                                                       kBase2WalkSpeed,
                                                                                       kBase3WalkSpeed,
                                                                                       kGathererWalkSpeed,
                                                                                       kGatherer2WalkSpeed,
                                                                                       kGatherer3WalkSpeed,
                                                                                       kFighterWalkSpeed,
                                                                                       kFighter2WalkSpeed,
                                                                                       kFighter3WalkSpeed};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kAttackSpeed = {kBaseAttackSpeed,
                                                                                         kBase2AttackSpeed,
                                                                                         kBase3AttackSpeed,
                                                                                         kGathererAttackSpeed,
                                                                                         kGatherer2AttackSpeed,
                                                                                         kGatherer3AttackSpeed,
                                                                                         kFighterAttackSpeed,
                                                                                         kFighter2AttackSpeed,
                                                                                         kFighter3AttackSpeed};

std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kAttackDamage = {kBaseAttackDamage,
                                                                                       kBase2AttackDamage,
                                                                                       kBase3AttackDamage,
                                                                                       kGathererAttackDamage,
                                                                                       kGatherer2AttackDamage,
                                                                                       kGatherer3AttackDamage,
                                                                                       kFighterAttackDamage,
                                                                                       kFighter2AttackDamage,
                                                                                       kFighter3AttackDamage};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kAttackRange = {kBaseAttackRange,
                                                                                         kBase2AttackRange,
                                                                                         kBase3AttackRange,
                                                                                         kGathererAttackRange,
                                                                                         kGatherer2AttackRange,
                                                                                         kGatherer3AttackRange,
                                                                                         kFighterAttackRange,
                                                                                         kFighter2AttackRange,
                                                                                         kFighter3AttackRange};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kSize = {kBaseSize,
                                                                                  kBase2Size,
                                                                                  kBase3Size,
                                                                                  kGathererSize,
                                                                                  kGatherer2Size,
                                                                                  kGatherer3Size,
                                                                                  kFighterSize,
                                                                                  kFighter2Size,
                                                                                  kFighter3Size};

std::array<double const, static_cast<std::size_t>(UnitType::NONE)> const kGatherSpeed = {kBaseGatherSpeed,
                                                                                         kBase2GatherSpeed,
                                                                                         kBase3GatherSpeed,
                                                                                         kGathererGatherSpeed,
                                                                                         kGatherer2GatherSpeed,
                                                                                         kGatherer3GatherSpeed,
                                                                                         kFighterGatherSpeed,
                                                                                         kFighter2GatherSpeed,
                                                                                         kFighter3GatherSpeed};

std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kGatherAmount = {kBaseGatherAmount,
                                                                                       kBase2GatherAmount,
                                                                                       kBase3GatherAmount,
                                                                                       kGathererGatherAmount,
                                                                                       kGatherer2GatherAmount,
                                                                                       kGatherer3GatherAmount,
                                                                                       kFighterGatherAmount,
                                                                                       kFighter2GatherAmount,
                                                                                       kFighter3GatherAmount};

// Unit cost requirements
// Resource requirements (resources/unit)
int const kBaseResourceCost = 100;
int const kBase2ResourceCost = 100;
int const kBase3ResourceCost = 100;
int const kGathererResourceCost = 0;
int const kGatherer2ResourceCost = 0;
int const kGatherer3ResourceCost = 0;
int const kFighterResourceCost = 0;
int const kFighter2ResourceCost = 0;
int const kFighter3ResourceCost = 0;

// Unit Type cost
UnitType const kBaseUnitTypeCost = UnitType::NONE;
UnitType const kBase2UnitTypeCost = UnitType::NONE;//UnitType::BASE_LV1;
UnitType const kBase3UnitTypeCost = UnitType::NONE;
UnitType const kGathererUnitTypeCost = UnitType::NONE;//UnitType::BASE_LV1;
UnitType const kGatherer2UnitTypeCost = UnitType::NONE;//UnitType::BASE_LV1;
UnitType const kGatherer3UnitTypeCost = UnitType::NONE;//UnitType::BASE_LV1;
UnitType const kFighterUnitTypeCost = UnitType::NONE;//UnitType::BASE_LV1;
UnitType const kFighter2UnitTypeCost = UnitType::NONE;//UnitType::FIGHTER_LV1;
UnitType const kFighter3UnitTypeCost = UnitType::NONE;//UnitType::FIGHTER_LV1;

// Unit cost requirements (units/new unit)
int const kBaseUnitCost = 0;
int const kBase2UnitCost = 0;
int const kBase3UnitCost = 0;
int const kGathererUnitCost = 0;
int const kGatherer2UnitCost = 0;
int const kGatherer3UnitCost = 0;
int const kFighterUnitCost = 0;
int const kFighter2UnitCost = 0;
int const kFighter3UnitCost = 0;

// Determines cost in resources of the unit type
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kResourceCost = {kBaseResourceCost,
                                                                                       kBase2ResourceCost,
                                                                                       kBase3ResourceCost,
                                                                                       kGathererResourceCost,
                                                                                       kGatherer2ResourceCost,
                                                                                       kGatherer3ResourceCost,
                                                                                       kFighterResourceCost,
                                                                                       kFighter2ResourceCost,
                                                                                       kFighter3ResourceCost};

// Determines the type of unit for "unit costs"
std::array<UnitType const, static_cast<std::size_t>(UnitType::NONE)> const kUnitTypeCost = {kBaseUnitTypeCost,
                                                                                            kBase2UnitTypeCost,
                                                                                            kBase3UnitTypeCost,
                                                                                            kGathererUnitTypeCost,
                                                                                            kGatherer2UnitTypeCost,
                                                                                            kGatherer3UnitTypeCost,
                                                                                            kFighterUnitTypeCost,
                                                                                            kFighter2UnitTypeCost,
                                                                                            kFighter3UnitTypeCost};

// Determines the cost in units
std::array<int const, static_cast<std::size_t>(UnitType::NONE)> const kUnitCost = {kBaseUnitCost,
                                                                                   kBase2UnitCost,
                                                                                   kBase3UnitCost,
                                                                                   kGathererUnitCost,
                                                                                   kGatherer2UnitCost,
                                                                                   kGatherer3UnitCost,
                                                                                   kFighterUnitCost,
                                                                                   kFighter2UnitCost,
                                                                                   kFighter3UnitCost};

// Tiles to use
// Defines the string representation of the corresponding menu entry
//std::array<std::string const, 1> const kTileStrings = {"BG_Tile1.png"};
std::size_t const tilemap_dimensions = 20; // 20x20 background tile map
double const kParallaxStrength = 4.0; // Determines how fast the background moves in relation to player position

#endif //BUBBLEGROW_COMMON_HPP

/**
 * Note: Use https://google-styleguide.googlecode.com/svn/trunk/cppguide.html (Google C++) style guide.
 */