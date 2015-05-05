#include "common.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

    // Setup game state
    auto world = std::make_shared<World>();
    auto player = world->AddPlayer();
    if (player) {
        player->name = "Test_Player";
        player->resources = 1000;
        player->CreateUnits(1000, UnitType::BASE);
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