#include "ECS.h"

int IComponent::nextId = 0;

int Entity::GetId() const
{
	return id;
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
	/// <summary>
	/// Lambda Function modern c++ way using iterators to remove entity from system
	/// </summary>
	/// <param name="entity"></param>
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}

const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}

void Registry::Update()
{
	// Add the entities that are waiting to be created to the active Systems
	for (auto entity : entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();
	//TODO: Remove the entities that are waiting to be killed from the active Systems
}

Entity Registry::CreateEntity()
{
	int entityId;
	entityId = numEntities++;
	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// Make sure the entityComponentSignatures vector can accomodate the new entity
	if(entityId >= static_cast<int>(entityComponentSignatures.size())) {
		entityComponentSignatures.resize(entityId + 1);
	}

	NPGE_INFO("Entity of Id : {0} Created!", entityId);

	return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto entityId = entity.GetId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto& system : systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
		if (isInterested) {
			system.second->AddEntityToSystem(entity);
		}
	}
}
