#include "common.hpp"
#include "World.hpp"
#include "Renderer.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

    // Setup game state
    auto world = std::make_shared<World>();
    auto player = world->AddPlayer();
    if (player) {

    } else {
        //return 1;
    }

    // Setup SFML renderer
    auto renderer = std::make_shared<Renderer>();
    renderer->world = world;
    renderer->player = player;

    // Start game loop
    bool running = true;
    while(running) {
        running = renderer->PollEvents();
        renderer->ProcessInputs();

        world->UpdateAndProcess(1.0);

        renderer->RenderWorld();
    }

    return 0;
}