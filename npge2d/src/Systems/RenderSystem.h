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
		// Sort all entities of our system by z-index
		struct RenderableEntity {
			TransformComponent transformComponent;
			SpriteComponent spriteComponent;
		};
		std::vector<RenderableEntity> renderableEntities;
		for (auto entity : GetSystemEntities()) {
			RenderableEntity rE;
			rE.spriteComponent = entity.GetComponent<SpriteComponent>();
			rE.transformComponent = entity.GetComponent<TransformComponent>();
			renderableEntities.emplace_back(rE);
		}

		// TODO: Find a better place to sort the entities based on zIndex in the Render System
		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
			return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
		});

		// Loop all entities that the system is interested in
		for (auto entity : renderableEntities) {
			const auto transform = entity.transformComponent;
			const auto sprite = entity.spriteComponent;

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