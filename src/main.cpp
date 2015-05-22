#include "common.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

int main() {
    // Initialize SFML renderer
    auto renderer = std::make_shared<Renderer>();

    // Start game loop
    bool running = true;
    sf::Clock clock;
    while(running and renderer->window->isOpen()) {
        running = renderer->PollEvents();
        renderer->ProcessInputs();

        // Only update the game if one is running (otherwise, the player is at the menu).
        double duration = clock.restart().asSeconds();
        if (renderer->mode == GameMode::IN_GAME)
            renderer->world->UpdateAndProcess(duration);

        renderer->RenderGame(duration);
    }

    return 0;
}