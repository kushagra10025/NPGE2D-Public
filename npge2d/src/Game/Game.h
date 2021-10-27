#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

const int FPS = 120;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game
{
private:
	bool isRunning;
	int millisecsPreviousFrame = 0;
	Logger logManager;
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;

public:
	Game();
	~Game();

	void Initialize();
	void Run();
	void ProcessInput();
	void Setup();
	void Update();
	void Render();
	void Destroy();

	int windowWidth;
	int windowHeight;
};

#endif // !GAME_H