#include "SpawnHelpers.h"
#include "../CCL.h"
#include "../DRAW/DrawComponents.h" 
#include "../GAME/GameComponents.h"
#include <iostream>
#include <map>
#include <string>

void SpawnGameEntity(entt::registry& registry, DRAW::ModelManager& modelManager, const std::string& modelName, entt::entity gameEntity)
{
	auto& sourceCollection = modelManager.modelDefinitions[modelName];

	auto& destCollection = registry.emplace<DRAW::MeshCollection>(gameEntity);

	destCollection.collider = sourceCollection.collider;

	if (sourceCollection.collider.extent.x > 0 ||
		sourceCollection.collider.extent.y > 0 ||
		sourceCollection.collider.extent.z > 0)
	{
		registry.emplace<GAME::Collidable>(gameEntity);
	}

	for (entt::entity sourceEntity : sourceCollection.meshes)
	{
		entt::entity renderEntity = registry.create();

		auto& geoData = registry.get<DRAW::GeometryData>(sourceEntity);
		auto& gpuInst = registry.get<DRAW::GPUInstance>(sourceEntity);


		registry.emplace<DRAW::GeometryData>(renderEntity, geoData);
		registry.emplace<DRAW::GPUInstance>(renderEntity, gpuInst);

		destCollection.meshes.push_back(renderEntity);

		if (!registry.any_of<GAME::Transform>(gameEntity))
		{
			registry.emplace<GAME::Transform>(gameEntity, GAME::Transform{ gpuInst.transform });
		}
	}
}
