#include "AssetStore.h"

AssetStore::AssetStore()
{
	NPGE_INFO("AssetStore constructor called!");
}

AssetStore::~AssetStore()
{
	ClearAssets();
	NPGE_INFO("AssetStore destructor called!");
}

void AssetStore::ClearAssets()
{
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Add texture to map
	textures.emplace(assetId, texture);

	NPGE_DEBUG("Texture added with Asset Id : {0}", assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
	// Check if assetId exists
	return textures[assetId];
}
