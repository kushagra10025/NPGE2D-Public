#ifndef ECS_H
#define ECS_H

#include "../Logger/Log.h"

#include <bitset>
#include <vector>
#include <set>
#include <unordered_map>
#include <typeindex>

const unsigned int MAX_COMPONENTS = 32;
/// <summary>
/// We use a bitset(1 & 0) to keep track of which components an entity has,
/// and also helps keep track of which entities a system is interested in
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
	static int nextId;
};

/// <summary>
/// Used to assign unique ID to T
/// </summary>
/// <typeparam name="T">Component Type</typeparam>
template <typename T>
class Component : public IComponent{
	/// <summary>
	/// Get Unique Id
	/// </summary>
	/// <returns>Returns T Unique ID</returns>
public:
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	int id;
public:
	Entity(int id) : id(id) {};
	Entity(const Entity& entity) = default;
	int GetId() const;
	// ...
	Entity& operator =(const Entity& other) = default;
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }

	template <typename T, typename ...TArgs> void AddComponent(TArgs&& ...args);
	template <typename T> void RemoveComponent();
	template <typename T> bool HasComponent() const;
	template <typename T> T& GetComponent() const;

	//Hold a pointer to Entitie's Owner Registry
	class Registry* registry;
};

/// <summary>
/// System processes entities that contain a specific signature
/// </summary>
class System {
private:
	Signature componentSignature;
	std::vector<Entity> entities;
public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	/// <summary>
	/// Entities must have T to be considered by the System
	/// </summary>
	/// <typeparam name="TComponent">Component Type</typeparam>
	template <typename TComponent> void RequireComponent();
};

/// <summary>
/// We inherit Pool from this class to remove providing type of pool T during creating of pool.
/// </summary>
class IPool {
public:
	virtual ~IPool() {}
};

/// <summary>
/// Pool is a vector to store T against the registry in a matrix
/// </summary>
/// <typeparam name="T">Component Type</typeparam>
template <typename T>
class Pool : public IPool {
private:
	std::vector<T> data;
public:
	Pool(int size = 100) {
		data.resize(size);
	}

	virtual ~Pool() = default;
	bool isEmpty() const { return data.empty(); }
	int GetSize() const { return data.size(); }
	void Resize(int n) { data.resize(n); }
	void Clear() { data.clear(); }

	void Add(T object) { data.push_back(object); }
	void Set(int index, T object) { data[index] = object; }
	T& Get(int index) { return static_cast<T&>(data[index]); }
	
	T& operator [](unsigned int index) { return data[index]; }
};

/// <summary>
/// Manages the creation and destruction of entities, as well as
/// adding systems and components to entities
/// </summary>
class Registry {
private:
	int numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component
	// [vector index = componentId], [pool index = entityid]
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures, determines which components are turned 'on' for an entity
	// [vector index = entityid]
	std::vector<Signature> entityComponentSignatures;

	// Map of active systems [index = system typeid]
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Set of entities that are flagged to be addred or removed in the next registry Update()
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;
public:
	Registry() = default;

	void Update();
	/// Managing Entities, Systems and Components

	/*
	* Entity Management
	*/
	Entity CreateEntity();

	/*
	* Component Management
	*/
	template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
	template <typename T> void RemoveComponent(Entity entity);
	template <typename T> bool HasComponent(Entity entity) const;
	template <typename T> T& GetComponent(Entity entity) const;

	/*
	* System Management
	*/
	template <typename T, typename ...TArgs> void AddSystem(TArgs&& ...args);
	template <typename T> void RemoveSystem();
	template <typename T> bool HasSystem() const;
	template <typename T> T& GetSystem() const;

	void AddEntityToSystems(Entity entity);
};


template<typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}

template<typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs && ...args)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	// If componentId > current size of ComponentPools, resize the vector
	if (componentId >= static_cast<int>(componentPools.size())) {
		componentPools.resize(componentId + 1, nullptr);
	}

	// If we still dont have a Pool for that component type
	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>();
		componentPools[componentId] = newComponentPool;
	}

	// Get the pool of component values for that component type
	std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

	// If the entity id > current size of component pool, resize the pool
	if (entityId >= static_cast<int>(componentPool->GetSize())) {
		componentPool->Resize(numEntities);
	}

	// Create a new component of T type and forward the various params to the constructor
	T newComponent(std::forward<TArgs>(args)...);

	// Add the new component to component pool list
	componentPool->Set(entityId, newComponent);

	entityComponentSignatures[entityId].set(componentId);

	NPGE_DEBUG("Component ID : {0} Added to Entity ID : {1}", componentId, entityId);
}

template<typename T>
void Registry::RemoveComponent(Entity entity)
{
	// Assuming Already exists
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	entityComponentSignatures[entityId].set(componentId, false);

	NPGE_DEBUG("Component ID : {0} Removed From Entity ID : {1}", componentId, entityId);
}

template<typename T>
bool Registry::HasComponent(Entity entity) const
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	return entityComponentSignatures[entityId].test(componentId);
}

template<typename T>
T& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template<typename T, typename ...TArgs>
void Registry::AddSystem(TArgs && ...args)
{
	std::shared_ptr<T> newSystem = std::make_shared<T>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(T)), newSystem));
}

template<typename T>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(T)));
	systems.erase(system);
}

template<typename T>
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(T))) != systems.end();
}

template<typename T>
T& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(T)));
	return *(std::static_pointer_cast<T>(system->second));
}

template<typename T, typename ...TArgs>
void Entity::AddComponent(TArgs && ...args)
{
	registry->AddComponent<T>(*this, std::forward<TArgs>(args)...);
}

template<typename T>
void Entity::RemoveComponent()
{
	registry->RemoveComponent<T>(*this);
}

template<typename T>
bool Entity::HasComponent() const
{
	return registry->HasComponent<T>(*this);
}

template<typename T>
T& Entity::GetComponent() const
{
	return registry->GetComponent<T>(*this);
}

#endif
