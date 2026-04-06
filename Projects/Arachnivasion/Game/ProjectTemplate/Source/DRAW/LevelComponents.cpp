#include "DrawComponents.h"
#include "../GAME/GameComponents.h"
#include "../CCL.h"

namespace DRAW
{

	void Construct_CPULevel(entt::registry& registry, entt::entity entity) 
	{
		auto& cpuLevel = registry.get<CPULevel>(entity);

		GW::SYSTEM::GLog log;
		log.Create("levelLoad");
		log.EnableConsoleLogging(true);

		cpuLevel.lvlData.LoadLevel(cpuLevel.levelFile.c_str(), cpuLevel.modelPath.c_str(), log);
	}

    void Construct_GPULevel(entt::registry& registry, entt::entity entity)
    {
        if (!registry.all_of<DRAW::CPULevel>(entity)) {
            std::cout << "GPULevel constructed without CPULevel on same entity\n";
            return;
        }

        auto& levelData = registry.get<CPULevel>(entity).lvlData;

        registry.emplace<DRAW::VulkanVertexBuffer>(entity);
        registry.emplace<std::vector<H2B::VERTEX>>(entity, levelData.levelVertices);
        registry.patch<DRAW::VulkanVertexBuffer>(entity);

        registry.emplace<DRAW::VulkanIndexBuffer>(entity);
        registry.emplace<std::vector<unsigned int>>(entity, levelData.levelIndices);
        registry.patch<DRAW::VulkanIndexBuffer>(entity);

        auto ctxView = registry.view<GW::SYSTEM::GWindow>();
        entt::entity ctxEntity = ctxView.front();
        auto& modelManager = registry.emplace_or_replace<ModelManager>(ctxEntity);

        // create meshes 
        for (int obj = 0; obj < levelData.blenderObjects.size(); obj++)
        {
            auto bObj = levelData.blenderObjects[obj];
            auto model = levelData.levelModels[bObj.modelIndex];

            MeshCollection tempCollection;
            tempCollection.collider = levelData.levelColliders[model.colliderIndex];

            for (int meshIdx = 0; meshIdx < model.meshCount; meshIdx++)
            {
                auto drawable = registry.create();
                auto meshData = levelData.levelMeshes[meshIdx + model.meshStart];
                GeometryData geoData;
                geoData.vertexStart = model.vertexStart;
                geoData.indexCount = meshData.drawInfo.indexCount;
                geoData.indexStart = model.indexStart + meshData.drawInfo.indexOffset;
                registry.emplace<GeometryData>(drawable, geoData);

                GPUInstance gpuInstance;
                gpuInstance.transform = levelData.levelTransforms[bObj.transformIndex];
                gpuInstance.matData = levelData.levelMaterials[model.materialStart + meshData.materialIndex].attrib;

                registry.emplace<GPUInstance>(drawable, gpuInstance);

                if (model.isDynamic)
                {
                    registry.emplace<DRAW::DoNotRender>(drawable);
                    tempCollection.meshes.push_back(drawable);
                }
            }

            if (model.isDynamic)
            {
                modelManager.modelDefinitions[bObj.blendername] = tempCollection;
            }
            else if (model.isCollidable)
            {
                auto wallEntity = registry.create();

                registry.emplace<GAME::Collidable>(wallEntity);
                registry.emplace<GAME::Obstacle>(wallEntity);

                registry.emplace<GAME::Transform>(wallEntity, GAME::Transform{ levelData.levelTransforms[bObj.transformIndex] });
                registry.emplace<DRAW::MeshCollection>(wallEntity, tempCollection);
            }
        }
    }

	void Destroy_ModelManager(entt::registry& registry, entt::entity entity)
	{
		auto& manager = registry.get<ModelManager>(entity);

		for (auto& collection : manager.modelDefinitions)
		{
			for (auto& meshEntity : collection.second.meshes)
			{
				if (registry.valid(meshEntity))
					registry.destroy(meshEntity);
			}
		}
		manager.modelDefinitions.clear();
	}

	CONNECT_COMPONENT_LOGIC() {
		// register the Window component's logic
		registry.on_construct<CPULevel>().connect<Construct_CPULevel>();
		registry.on_construct<GPULevel>().connect<Construct_GPULevel>();

	}

}
