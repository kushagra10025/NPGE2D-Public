#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>
#include <SDL.h>

struct SpriteComponent {
	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_Rect srcRect;

	SpriteComponent(std::string assetId = "", int width = 5, int height = 5,int zIndex = 0, int srcRectX = 0, int srcRectY = 0) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->srcRect = { srcRectX, srcRectY, width, height };
		this->zIndex = zIndex;
	}
};

#endif