#pragma once

#include "../CCL.h"
#include "../DRAW/DrawComponents.h" 
#include "../GAME/GameComponents.h" 
#include <string>

void SpawnGameEntity(
    entt::registry& registry,
    DRAW::ModelManager& modelManager,
    const std::string& modelName,
    entt::entity gameEntity
);
