#include <functional> 
#include "../CCL.h"
#include "GameComponents.h"
#include "../DRAW/DrawComponents.h"
#include "../UTIL/Utilities.h"


namespace GAME
{
	void Update_Player(entt::registry& registry, entt::entity entity)
	{
		auto& input = registry.ctx().get<UTIL::Input>();
		auto& dt = registry.ctx().get<UTIL::DeltaTime>();
		auto config = registry.ctx().get<UTIL::Config>().gameConfig;

		auto& transform = registry.get<GAME::Transform>(entity);
		float enemyLimit;
		float margin;
		float speed;
		// ------------------ read config ------------------
		if (config) {
			speed = (*config).at("Player").at("speed").as<float>();

			//get enemy limit and margin from config
			if (config->find("Enemy1") != config->end())
			{
				auto& enemyCfg = (*config).at("Enemy1");
				if (enemyCfg.find("xBoundary") != enemyCfg.end())
					enemyLimit = enemyCfg.at("xBoundary").as<float>();
			}

			if (config->find("Player") != config->end())
			{
				auto& playerCfg = (*config).at("Player");
				if (playerCfg.find("xBoundaryMargin") != playerCfg.end())
					margin = playerCfg.at("xBoundaryMargin").as<float>();
			}
		}
		// ------------------ input ------------------
		float xChange = 0.0f;
		float state = 0.0f;

		input.immediateInput.GetState(G_KEY_D, state);
		if (state > 0) xChange += 1.0f;

		input.immediateInput.GetState(G_KEY_A, state);
		if (state > 0) xChange -= 1.0f;

		// ------------------ movement ------------------
		if (xChange != 0.0f)
		{
			GW::MATH::GVECTORF translation = { xChange, 0.0f, 0.0f, 0.0f };
			GW::MATH::GVector::NormalizeF(translation, translation);

			float scale = speed * (float)dt.dtSec;
			GW::MATH::GVector::ScaleF(translation, scale, translation);

			GW::MATH::GMatrix::TranslateGlobalF(
				transform.matrix,
				translation,
				transform.matrix
			);
		}

		// ------------------ CLAMP PLAYER X ------------------
		float playerLimit = enemyLimit + margin;
		float& x = transform.matrix.row4.x;

		if (x > playerLimit) x = playerLimit;
		if (x < -playerLimit) x = -playerLimit;
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GAME::Player>().connect<Update_Player>();
	}
}
