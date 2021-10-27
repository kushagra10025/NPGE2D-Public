#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../Logger/Log.h"

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System
{
private:
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(double deltaTime) {
		// TODO: 
		// Loop all entities that the system is interested in
		for (auto entity : GetSystemEntities()) {
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			//NPGE_DEBUG("Entity ID : {0} -> Position [{1},{2}]", entity.GetId(), transform.position.x, transform.position.y);
		}
	}
};

#endif