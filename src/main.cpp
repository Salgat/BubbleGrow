#include "common.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

    // Setup game state
    auto world = std::make_shared<World>();
    auto player = world->AddPlayer();
    auto enemy_player = world->AddPlayer();
    if (player and enemy_player) {
        player->position = sf::Vector2f(0.0, 0.0);
        player->name = "Test_Player";
        player->resources = 1000;
        player->CreateUnits(5, UnitType::BASE);

        enemy_player->position = sf::Vector2f(10.0, 10.0);
        enemy_player->name = "Test_Computer_Player";
        enemy_player->resources = 1000;
        enemy_player->CreateUnits(2, UnitType::BASE);
    } else {
        //return 1;
    }

    // Setup SFML renderer
    auto renderer = std::make_shared<Renderer>();
    renderer->world = world;
    renderer->player = player;

    // Start game loop
    bool running = true;
    sf::Clock clock;
    while(running and renderer->window->isOpen()) {
        running = renderer->PollEvents();
        renderer->ProcessInputs();

        world->UpdateAndProcess(clock.restart().asSeconds());

        renderer->RenderWorld();
    }

    return 0;
}