#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../Logger/Log.h"

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

#include <SDL.h>

class RenderSystem : public System
{
private:
public:
	RenderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
		// TODO: Sort all entities of our system by z-index
		

		// Loop all entities that the system is interested in
		for (auto entity : GetSystemEntities()) {
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			// Set Source and Destination Rectangle of sprite
			SDL_Rect srcRect = sprite.srcRect;
			SDL_Rect destRect = {
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				static_cast<int>(sprite.width * transform.scale.x),
				static_cast<int>(sprite.height * transform.scale.y)
			};

			// Draw the PNG Texture
			SDL_RenderCopyEx(
				renderer, 
				assetStore->GetTexture(sprite.assetId),
				&srcRect,
				&destRect,
				transform.rotation,
				NULL,
				SDL_FLIP_NONE
			);

			// SDL Rect Drawing
			/*SDL_Rect objRect = { 
				static_cast<int>(transform.position.x),
				static_cast<int>(transform.position.y),
				sprite.width, 
				sprite.height 
			};

			SDL_SetRenderDrawColor(renderer, 255,255,255,255);
			SDL_RenderFillRect(renderer, &objRect);*/
		}
	}
};

#endif