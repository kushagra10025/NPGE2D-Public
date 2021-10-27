#include "Game.h"
#include <iostream>
#include <fstream>

#include "../Logger/Log.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"


Game::Game()
{
	isRunning = false;
	logManager.Initialize();

	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();

	NPGE_INFO("NegProt\'s Game Engine 2D");
}

Game::~Game()
{
	NPGE_INFO("Game Engine Destructor"); 
	logManager.Destroy();
}

void Game::Initialize()
{
	// LOG MANAGER INIT (Here or in Constructor)
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		NPGE_CRITICAL("Error Initializing SDL");
		return;
	}
	// Get Current Screen Max Window and Max Height
	// Create FakeFullScreen
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = 800; //displayMode.w;
	windowHeight = 600; //displayMode.h;

	 window = SDL_CreateWindow(
		"Negative Proton\'s Game Engine 2D",
		//NULL,
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		NPGE_CRITICAL("Error Creating SDL Window");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		NPGE_CRITICAL("Error Creating SDL Renderer");
		return;
	}

	// Create real fullscreen - Change Videomode
	// TURN ON REAL FULL SCREEN
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	isRunning = true;
}

void Game::Run()
{
	NPGE_INFO("NegProt's Game Engine 2D");
	NPGE_INFO("FPS Capped At : {0}", FPS);
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
		}
	}
}

void Game::Setup() {
	// Add Systems that need to be processed in our game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();

	// Adding ass3ets to asset store
	assetStore->AddTexture(renderer,"tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer,"truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "tilemap-texture", "./assets/tilemaps/jungle.png");

	// Load the tilemap
	int tileSize = 32;
	double tileScale = 1.0;
	int mapNumCols = 25;
	int mapNumRows = 20;
	// Load map file
	std::fstream mapFile;
	std::string STRING;
	mapFile.open("./assets/tilemaps/jungle.map");
	for (int y = 0; y < mapNumRows; y++)
	{
		for (int x = 0; x < mapNumCols; x++)
		{
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();
			// One entity per tile
			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize),y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-texture",tileSize, tileSize,0,srcRectX,srcRectY);
		}
	}
	mapFile.close();

	// Create Entity
	Entity tank = registry->CreateEntity();
	Entity truck = registry->CreateEntity();

	// Add some components to that entity
	tank.AddComponent<TransformComponent>(glm::vec2(50, 100), glm::vec2(2, 2), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32,1);

	truck.AddComponent<TransformComponent>(glm::vec2(50, 100), glm::vec2(2, 2), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0, 0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32,2);

	 //AddComponent to Tank - Transform, BoxCollider, SpriteComponent
}

void Game::Update()
{
	// If too fast, waste time till we reach MILLISECS_PER_FRAME
	// By This if there is no need to Limit FPS in game
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecsPreviousFrame + MILLISECS_PER_FRAME));
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// Difference in ticsk since last frame in secs.
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	//Store current frame time
	millisecsPreviousFrame = SDL_GetTicks();

	// Invoke all the systems that need to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	// Update Collision System
	// Update Damage System

	// Update the registry to process the entities that are waiting to be creating/removed
	registry->Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 0);
	SDL_RenderClear(renderer);
	
	// Render Game Objects

	// Invoke all the systems that need to render
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
	
	// Back and Front Buffer Swap
	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	// LOG MANAGER DESTROY (Here or in Destructor)

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
