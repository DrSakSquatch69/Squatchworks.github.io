#include "GameComponents.h"
#include "../DRAW/DrawComponents.h"
#include "../UTIL/Utilities.h"
#include "../CCL.h"
#include "SpawnHelpers.h"
#include <algorithm>

namespace GAME
{

	static int GetEggSacPoints(const ini::IniFile& cfg)
	{
		const std::string section = "EggSac";
		const std::string key = "points";

		if (cfg.find(section) == cfg.end())
			return 100;

		const auto& egg = cfg.at(section);
		if (egg.find(key) == egg.end())
			return 100;

		return egg.at(key).as<int>();
	}


	void BounceEnemy(GW::MATH::GVECTORF enemyPos, GAME::Velocity& velocity, GW::MATH::GOBBF wallOBB)
	{
		GW::MATH::GVECTORF closestPoint;
		GW::MATH::GCollision::ClosestPointToOBBF(wallOBB, enemyPos, closestPoint);

		GW::MATH::GVECTORF normal;
		GW::MATH::GVector::SubtractVectorF(enemyPos, closestPoint, normal);
		normal.y = 0.0f;
		normal.w = 0.0f;
		GW::MATH::GVector::NormalizeF(normal, normal);

		float dot = 0.0f;
		GW::MATH::GVector::DotF(velocity.direction, normal, dot);

		GW::MATH::GVECTORF temp;
		GW::MATH::GVector::ScaleF(normal, 2.0f * dot, temp);
		GW::MATH::GVector::SubtractVectorF(velocity.direction, temp, velocity.direction);
	}

	void Update_GameManager(entt::registry& registry, entt::entity entity)
	{
		if (registry.any_of<GAME::GameOver>(entity))
			return;

		if (!registry.view<GAME::GameOver>().empty())
			return;
		auto playerView = registry.view<GAME::Player>();
		for (auto entity : playerView) {
			registry.patch<GAME::Player>(entity);
		}

		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		float speedStep = (*config).at("Enemy1").at("speedStep").as<float>();
		float maxSpeed = (*config).at("Enemy1").at("maxSpeed").as<float>();

		auto swarmView = registry.view<GAME::GameManager, GAME::Swarm>();
		entt::entity swarmEntity = (swarmView.begin() == swarmView.end())
			? entt::null
			: *swarmView.begin();

// --------------- ensure all enemies have the correct speed even when we spawn new waves
		if (swarmEntity != entt::null) {
			const float swarmSpeed = registry.get<GAME::Swarm>(swarmEntity).speed;
			auto enemyMoveView = registry.view<GAME::Enemy, GAME::Velocity>();
			for (auto e : enemyMoveView) {
				registry.get<GAME::Velocity>(e).speed = swarmSpeed;
			}
		}

		// ---------------------------------- EggSac (UFO) spawning ------------------------
		float dt = 0.0f;
		if (registry.ctx().contains<UTIL::DeltaTime>())
			dt = (float)registry.ctx().get<UTIL::DeltaTime>().dtSec;

		if (registry.any_of<GAME::EggSacSpawner>(entity))
		{
			auto& spawner = registry.get<GAME::EggSacSpawner>(entity);
			if (spawner.active)
			{
				spawner.timer -= dt;

				if (spawner.timer <= 0.0f)
				{
					auto& cfg = *registry.ctx().get<UTIL::Config>().gameConfig;

					if (cfg.find("EggSac") != cfg.end())
					{
						auto& eggCfg = cfg.at("EggSac");

						std::string modelName = eggCfg.at("model").as<std::string>();
						int hp = eggCfg.at("hitpoints").as<int>();
						float speed = eggCfg.at("speed").as<float>();

						float intervalMin = eggCfg.at("spawnIntervalMin").as<float>();
						float intervalMax = eggCfg.at("spawnIntervalMax").as<float>();

						int randomSide = eggCfg.at("randomSide").as<int>();

						float laneZ = eggCfg.at("laneZ").as<float>();
						float startXBase = eggCfg.at("startX").as<float>();
						float scale = eggCfg.at("scale").as<float>();

						int oneAtATime = 1;
						if (eggCfg.find("oneAtATime") != eggCfg.end())
							oneAtATime = eggCfg.at("oneAtATime").as<int>();

						// random next spawn time
						auto Rand01 = []() -> float { return (float)rand() / (float)RAND_MAX; };
						auto RandRange = [&](float a, float b) -> float { return a + (b - a) * Rand01(); };
						spawner.timer = RandRange(intervalMin, intervalMax);

						if (oneAtATime && !registry.view<GAME::EggSac>().empty())
							return;

						float startX = startXBase;
						if (randomSide)
						{
							bool spawnLeft = (Rand01() < 0.5f);
							startX = spawnLeft ? -std::abs(startXBase) : std::abs(startXBase);
						}

						entt::entity egg = registry.create();
						registry.emplace<GAME::EggSac>(egg);
						registry.emplace<GAME::Health>(egg, GAME::Health{ hp });

						// movement
						float dirX = (startX < 0.0f) ? 1.0f : -1.0f;
						GW::MATH::GVECTORF dir = { dirX, 0.0f, 0.0f, 0.0f };
						registry.emplace<GAME::Velocity>(egg, GAME::Velocity{ dir, speed });

						GW::MATH::GMATRIXF m;
						GW::MATH::GMatrix::IdentityF(m);
						GW::MATH::GVECTORF s = { scale, scale, scale, 0.0f };
						GW::MATH::GMatrix::ScaleLocalF(m, s, m);
						m.row4.x = startX;
						m.row4.z = laneZ;
						registry.emplace<GAME::Transform>(egg, GAME::Transform{ m });
						auto ctxView = registry.view<GW::SYSTEM::GWindow>();
						entt::entity ctxEntity = ctxView.front();
						auto& modelManager = registry.get<DRAW::ModelManager>(ctxEntity);

						int eggPoints = GetEggSacPoints(*registry.ctx().get<UTIL::Config>().gameConfig);
						registry.emplace<GAME::PointValue>(egg, GAME::PointValue{ eggPoints });

						SpawnGameEntity(registry, modelManager, modelName, egg);

						std::cout << "EggSac Spawned at X: " << startX << " Z: " << laneZ << std::endl;

						if (registry.ctx().contains<GAME::AudioBank>()) {
							auto& bank = registry.ctx().get<GAME::AudioBank>();
							if (bank.enabled) bank.eggSpawn.Play();
						}
					}
					else
					{
						spawner.timer = 10.0f;
					}
				}
			}
		}

		// ----------------------------- EggSac despawn ------------------------

		{
			float despawnLimit;
			if (config && config->find("EggSac") != config->end())
			{
				auto& enemyCfg = (*config).at("EggSac");
				if (enemyCfg.find("despawnLimit") != enemyCfg.end())
					despawnLimit = enemyCfg.at("despawnLimit").as<float>();
			}
			
			auto eggView = registry.view<GAME::EggSac, GAME::Transform>();

			for (auto egg : eggView)
			{
				const auto& t = registry.get<GAME::Transform>(egg);

				if (t.matrix.row4.x > despawnLimit || t.matrix.row4.x < -despawnLimit)
				{
					registry.emplace_or_replace<GAME::ToDestroy>(egg);
				}
			}
		}


		//			----------------- collision detection ------------------------
		auto collisionView = registry.view<GAME::Collidable, GAME::Transform, DRAW::MeshCollection>();

		for (auto itA = collisionView.begin(); itA != collisionView.end(); ++itA)
		{
			auto entityA = *itA;
			auto& transA = registry.get<GAME::Transform>(entityA).matrix;
			auto& meshA = registry.get<DRAW::MeshCollection>(entityA);
			GW::MATH::GOBBF obbA = meshA.collider;

			GW::MATH::GVECTORF scaleA;
			GW::MATH::GMatrix::GetScaleF(transA, scaleA);
			obbA.extent.x *= scaleA.x; obbA.extent.y *= scaleA.y; obbA.extent.z *= scaleA.z;
			GW::MATH::GQUATERNIONF quatA;
			GW::MATH::GQuaternion::SetByMatrixF(transA, quatA);
			GW::MATH::GQuaternion::MultiplyQuaternionF(quatA, obbA.rotation, obbA.rotation);
			GW::MATH::GVECTORF worldCenterA = obbA.center;
			GW::MATH::GMatrix::VectorXMatrixF(transA, worldCenterA, worldCenterA);
			worldCenterA.x += transA.row4.x;
			worldCenterA.y += transA.row4.y;
			worldCenterA.z += transA.row4.z;
			obbA.center = worldCenterA;

			auto itB = itA;
			++itB;
			while (itB != collisionView.end())
			{
				auto entityB = *itB;
				auto& transB = registry.get<GAME::Transform>(entityB).matrix;
				auto& meshB = registry.get<DRAW::MeshCollection>(entityB);
				GW::MATH::GOBBF obbB = meshB.collider;

				GW::MATH::GVECTORF scaleB;
				GW::MATH::GMatrix::GetScaleF(transB, scaleB);
				obbB.extent.x *= scaleB.x; obbB.extent.y *= scaleB.y; obbB.extent.z *= scaleB.z;
				GW::MATH::GQUATERNIONF quatB;
				GW::MATH::GQuaternion::SetByMatrixF(transB, quatB);
				GW::MATH::GQuaternion::MultiplyQuaternionF(quatB, obbB.rotation, obbB.rotation);
				GW::MATH::GVECTORF worldCenterB = obbB.center;
				GW::MATH::GMatrix::VectorXMatrixF(transB, worldCenterB, worldCenterB);
				worldCenterB.x += transB.row4.x;
				worldCenterB.y += transB.row4.y;
				worldCenterB.z += transB.row4.z;
				obbB.center = worldCenterB;

				GW::MATH::GCollision::GCollisionCheck result;
				GW::MATH::GCollision::TestOBBToOBBF(obbA, obbB, result);

				if (result == GW::MATH::GCollision::GCollisionCheck::COLLISION)
				{
					bool aIsBullet = registry.any_of<GAME::Bullet>(entityA);
					bool bIsObstacle = registry.any_of<GAME::Obstacle>(entityB);
					if (aIsBullet && bIsObstacle)
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);

					bool bIsBullet = registry.any_of<GAME::Bullet>(entityB);
					bool aIsObstacle = registry.any_of<GAME::Obstacle>(entityA);
					if (bIsBullet && aIsObstacle)
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);

					if (registry.any_of<GAME::Bullet>(entityA) && registry.any_of<GAME::Enemy>(entityB))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);
						if (registry.any_of<GAME::Health>(entityB)) {
							registry.get<GAME::Health>(entityB).value -= 1;
						}
					}

					if (registry.any_of<GAME::Bullet>(entityB) && registry.any_of<GAME::Enemy>(entityA))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);
						if (registry.any_of<GAME::Health>(entityA)) {
							registry.get<GAME::Health>(entityA).value -= 1;
						}
					}

					if (registry.any_of<GAME::Player>(entityA) && registry.any_of<GAME::Enemy>(entityB))
					{
						if (!registry.any_of<GAME::Invulnerability>(entityA))
						{
							if (registry.any_of<GAME::Health>(entityA)) {
								auto& hp = registry.get<GAME::Health>(entityA);
								hp.value -= 1;
								std::cout << "Player Health: " << hp.value << "\n";
							}

							std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
							float invulnDuration = (*config).at("Player").at("invulnPeriod").as<float>();

							registry.emplace_or_replace<GAME::Invulnerability>(entityA, GAME::Invulnerability{ invulnDuration });
						}
					}

					if (registry.any_of<GAME::Player>(entityB) && registry.any_of<GAME::Enemy>(entityA))
					{
						if (!registry.any_of<GAME::Invulnerability>(entityB))
						{
							if (registry.any_of<GAME::Health>(entityB)) {
								auto& hp = registry.get<GAME::Health>(entityB);
								hp.value -= 1;
								std::cout << "Player Health: " << hp.value << "\n";
							}

							std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
							float invulnDuration = (*config).at("Player").at("invulnPeriod").as<float>();

							registry.emplace_or_replace<GAME::Invulnerability>(entityB, GAME::Invulnerability{ invulnDuration });
						}
					}

					if (registry.any_of<GAME::Bullet>(entityA) && registry.any_of<GAME::EggSac>(entityB))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);

						if (registry.any_of<GAME::Health>(entityB))
						{
							auto& hp = registry.get<GAME::Health>(entityB);
							hp.value -= 1;

							if (hp.value <= 0)
								registry.emplace_or_replace<GAME::KilledByPlayer>(entityB);
						}
					}

					if (registry.any_of<GAME::Bullet>(entityB) && registry.any_of<GAME::EggSac>(entityA))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);

						if (registry.any_of<GAME::Health>(entityA))
						{
							auto& hp = registry.get<GAME::Health>(entityA);
							hp.value -= 1;

							if (hp.value <= 0)
								registry.emplace_or_replace<GAME::KilledByPlayer>(entityA);
						}
					}
					// Enemy bullet hits player
					if (registry.any_of<GAME::EnemyBullet>(entityA) &&
						registry.any_of<GAME::Player>(entityB))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);

						if (!registry.any_of<GAME::Invulnerability>(entityB) &&
							registry.any_of<GAME::Health>(entityB))
						{
							registry.get<GAME::Health>(entityB).value -= 1;

							float invuln = (*config).at("Player").at("invulnPeriod").as<float>();
							registry.emplace_or_replace<GAME::Invulnerability>(entityB, GAME::Invulnerability{ invuln });
						}
					}

					if (registry.any_of<GAME::EnemyBullet>(entityB) &&
						registry.any_of<GAME::Player>(entityA))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);

						if (!registry.any_of<GAME::Invulnerability>(entityA) &&
							registry.any_of<GAME::Health>(entityA))
						{
							registry.get<GAME::Health>(entityA).value -= 1;

							float invuln = (*config).at("Player").at("invulnPeriod").as<float>();
							registry.emplace_or_replace<GAME::Invulnerability>(entityA, GAME::Invulnerability{ invuln });
						}
					}


					// Bunker collision
					if (registry.any_of<GAME::Bullet>(entityA) && registry.any_of<GAME::Bunker>(entityB) &&
						!registry.any_of<GAME::ToDestroy>(entityA))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);
						if (registry.any_of<GAME::Health>(entityB))
							registry.get<GAME::Health>(entityB).value -= 1;

						if (registry.get<GAME::Health>(entityB).value <= 0)
							registry.emplace_or_replace<GAME::ToDestroy>(entityB);
					}

					if (registry.any_of<GAME::Bullet>(entityB)&& registry.any_of<GAME::Bunker>(entityA) &&
						!registry.any_of<GAME::ToDestroy>(entityB))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);
						if (registry.any_of<GAME::Health>(entityA))
							registry.get<GAME::Health>(entityA).value -= 1;

						if (registry.get<GAME::Health>(entityA).value <= 0)
							registry.emplace_or_replace<GAME::ToDestroy>(entityA);
					}

					if (registry.any_of<GAME::EnemyBullet>(entityA) && registry.any_of<GAME::Bunker>(entityB) &&
						!registry.any_of<GAME::ToDestroy>(entityA))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityA);
						if (registry.any_of<GAME::Health>(entityB))
							registry.get<GAME::Health>(entityB).value -= 1;

						if (registry.get<GAME::Health>(entityB).value <= 0)
							registry.emplace_or_replace<GAME::ToDestroy>(entityB);
					}

					if (registry.any_of<GAME::EnemyBullet>(entityB) && registry.any_of<GAME::Bunker>(entityA) &&
						!registry.any_of<GAME::ToDestroy>(entityB))
					{
						registry.emplace_or_replace<GAME::ToDestroy>(entityB);
						if (registry.any_of<GAME::Health>(entityA))
							registry.get<GAME::Health>(entityA).value -= 1;

						if (registry.get<GAME::Health>(entityA).value <= 0)
							registry.emplace_or_replace<GAME::ToDestroy>(entityA);
					}

				}
				++itB;
			}
		}
		// ----------------------------- This is the enemy movement. side to side and down --------------------------
		bool hiveMindFlag = false;
		float screenLimit;
		if (config && config->find("Enemy1") != config->end())
		{
			auto& enemyCfg = (*config).at("Enemy1");
			if (enemyCfg.find("xBoundary") != enemyCfg.end())
				screenLimit = enemyCfg.at("xBoundary").as<float>();
		}  // we change this to change the screen limit for enemy movement=======

		auto enemyCheckView = registry.view<GAME::Enemy, GAME::Transform, GAME::Velocity>();

		for (auto entity : enemyCheckView)
		{
			auto& trans = registry.get<GAME::Transform>(entity);

			auto& vel = registry.get<GAME::Velocity>(entity);

			if (std::abs(trans.matrix.row4.x) > screenLimit)
			{
				if ((trans.matrix.row4.x > 0 && vel.direction.x > 0) ||
					(trans.matrix.row4.x < 0 && vel.direction.x < 0))
				{
					hiveMindFlag = true;
					break;
				}
			}
		}

		if (hiveMindFlag)
		{
			for (auto entity : enemyCheckView)
			{
				auto& vel = registry.get<GAME::Velocity>(entity);
				auto& trans = registry.get<GAME::Transform>(entity);

				vel.direction.x *= -1.0f;

				trans.matrix.row4.z -= 2.0f;

				GW::MATH::GVECTORF currentPos = trans.matrix.row4;
				GW::MATH::GVECTORF currentScale;
				GW::MATH::GMatrix::GetScaleF(trans.matrix, currentScale);

				GW::MATH::GMatrix::IdentityF(trans.matrix);

				GW::MATH::GMatrix::ScaleLocalF(trans.matrix, currentScale, trans.matrix);
				trans.matrix.row4 = currentPos;
			}
		}
		//-------------------- count killed enemies ------------------------
		int killedEnemies = 0;
		{
			auto killedView = registry.view<GAME::CountedKill>();
			killedEnemies = (int)std::distance(killedView.begin(), killedView.end());
		}
		
		// --------------------------- enemy firing (Space Invaders style) ------------------------
		std::unordered_map<int, entt::entity> bottomMost;

		// ----------------------- find bottom-most enemy per column ----------
		{
			auto enemyView = registry.view<GAME::Enemy, GAME::Transform>();
			float spacingX = (*config).at("Enemy1").at("spacingX").as<float>();

			for (auto e : enemyView)
			{
				const auto& t = registry.get<GAME::Transform>(e);
				int column = (int)std::round(t.matrix.row4.x / spacingX);

				if (!bottomMost.count(column) ||
					t.matrix.row4.z < registry.get<GAME::Transform>(bottomMost[column]).matrix.row4.z)
				{
					bottomMost[column] = e;
				}
			}
		}

		// ------------------ global enemy fire controller ----------
		{
			auto& cfg = *registry.ctx().get<UTIL::Config>().gameConfig;
			if (cfg.find("EnemyBullet") == cfg.end())
				goto AfterEnemyFiring;

			auto& controller = registry.get<GAME::EnemyFireController>(entity);

			const auto& bulletCfg = cfg.at("EnemyBullet");

			float bulletSpeed = bulletCfg.at("speed").as<float>();
			float fireInterval = bulletCfg.at("fireInterval").as<float>();
			std::string bulletModel = bulletCfg.at("model").as<std::string>();

			//  difficulty ramp 
			controller.timer -= dt;
			if (controller.timer > 0.0f)
				goto AfterEnemyFiring;

			controller.timer = (std::max)(0.4f, fireInterval - killedEnemies * 0.05f);

			if (bottomMost.empty())
				goto AfterEnemyFiring;

			// pick ONE random bottom-most spider
			auto it = bottomMost.begin();
			std::advance(it, rand() % bottomMost.size());
			entt::entity shooter = it->second;

			// spawn enemy bullet
			entt::entity bullet = registry.create();
			registry.emplace<GAME::EnemyBullet>(bullet);

			const auto& eTrans = registry.get<GAME::Transform>(shooter);

			GW::MATH::GMATRIXF bulletMatrix;
			GW::MATH::GMatrix::IdentityF(bulletMatrix);
			bulletMatrix.row4 = eTrans.matrix.row4;

			registry.emplace<GAME::Transform>(bullet, GAME::Transform{ bulletMatrix });

			GW::MATH::GVECTORF dir = { 0.0f, 0.0f, -1.0f, 0.0f };
			registry.emplace<GAME::Velocity>(bullet, GAME::Velocity{ dir, bulletSpeed });

			auto ctxView = registry.view<GW::SYSTEM::GWindow>();
			entt::entity ctxEntity = ctxView.front();
			auto& modelManager = registry.get<DRAW::ModelManager>(ctxEntity);
			SpawnGameEntity(registry, modelManager, bulletModel, bullet);
		}

	AfterEnemyFiring:

		// ---------------------- enemy death and speed ramping --------------------------
		auto statusView = registry.view<GAME::Health, GAME::Enemy>();

		for (auto enemy : statusView)
		{
			if (registry.get<GAME::Health>(enemy).value <= 0)
			{
				if (registry.ctx().contains<GAME::AudioBank>()) {
					auto& bank = registry.ctx().get<GAME::AudioBank>();
					if (bank.enabled) bank.enemyDie.Play();
				}
				// count kill once
				if (!registry.any_of<GAME::CountedKill>(enemy))
				{
					registry.emplace<GAME::CountedKill>(enemy);

					if (swarmEntity != entt::null && registry.valid(swarmEntity))
					{
						auto& swarm = registry.get<GAME::Swarm>(swarmEntity);
						swarm.speed = (std::min)(swarm.speed + speedStep, maxSpeed);
					}
				}

				registry.emplace_or_replace<GAME::ToDestroy>(enemy);
			}
		}
			//-------------- bullet out-of-bounds cleanup --------------------------
		auto bulletView = registry.view<GAME::Bullet, GAME::Transform>();
		auto enemyBulletView = registry.view<GAME::EnemyBullet, GAME::Transform>();

			const float bulletMaxZ = 45.0f;   // bullet flew past enemies
			const float bulletMinZ = -45.0f;  // safety fallback

			for (auto bullet : bulletView)
			{
				const auto& transform = registry.get<GAME::Transform>(bullet);

				if (transform.matrix.row4.z > bulletMaxZ ||
					transform.matrix.row4.z < bulletMinZ)
				{
					registry.emplace_or_replace<GAME::ToDestroy>(bullet);
				}
			}

			for (auto bullet : enemyBulletView)
			{
				const auto& transform = registry.get<GAME::Transform>(bullet);

				if (transform.matrix.row4.z > bulletMaxZ ||
					transform.matrix.row4.z < bulletMinZ)
				{
					registry.emplace_or_replace<GAME::ToDestroy>(bullet);
				}
			}
			//------------------- PLAYER DEATH / RESPAWN ----------------
			auto playerLifeView = registry.view<GAME::Player, GAME::Health, GAME::Lives>();

			for (auto player : playerLifeView)
			{
				auto& health = registry.get<GAME::Health>(player);
				auto& lives = registry.get<GAME::Lives>(player);

				if (health.value <= 0)
				{
					lives.value -= 1;

					if (lives.value > 0)
					{
						// reset health
						auto config = registry.ctx().get<UTIL::Config>().gameConfig;
						int maxHealth = (*config).at("Player").at("hitpoints").as<int>();
						health.value = maxHealth;

						// respawn player (center bottom)
						auto& transform = registry.get<GAME::Transform>(player);
						transform.matrix.row4.x = 0.0f;
						transform.matrix.row4.z = -28.0f;

						// brief invulnerability
						float invuln = (*config).at("Player").at("invulnPeriod").as<float>();
						registry.emplace_or_replace<GAME::Invulnerability>(
							player, GAME::Invulnerability{ invuln });

						std::cout << "Life lost! Lives remaining: " << lives.value << "\n";
					}
					else
					{
						// no lives left → game over
						registry.emplace<GAME::GameOver>(player);
						std::cout << "No lives remaining. Game Over.\n";
					}
				}
			}
		//			------------------ eggsac death ------------------------
			auto eggView = registry.view<GAME::Health, GAME::EggSac>();
			for (auto egg : eggView)
			{
				if (registry.get<GAME::Health>(egg).value <= 0)
				{
					const bool killedByPlayer = registry.any_of<GAME::KilledByPlayer>(egg);

					if (killedByPlayer)
					{
						if (registry.ctx().contains<GAME::AudioBank>()) {
							auto& bank = registry.ctx().get<GAME::AudioBank>();
							if (bank.enabled) bank.eggDie.Play();
						}
					}

					registry.emplace_or_replace<GAME::ToDestroy>(egg);
				}
			}

		//			--------------- update GPUInstance transforms ------------------------
		auto view = registry.view<GAME::Transform, DRAW::MeshCollection>();
		for (auto entity : view)
		{
			const auto& gameTransform = registry.get<GAME::Transform>(entity);
			const auto& collection = registry.get<DRAW::MeshCollection>(entity);

			for (auto meshEntity : collection.meshes)
			{
				registry.patch<DRAW::GPUInstance>(meshEntity,
					[&gameTransform](DRAW::GPUInstance& gpuInst) {
						gpuInst.transform = gameTransform.matrix;
					});
			}
		}
		//			------------------ destroy entities ------------------------
		auto toDestroyView = registry.view<GAME::ToDestroy>();
		for (auto destroyEntity : toDestroyView)
		{
			//If this is a bullet, clear HasBullet from player
			if (registry.any_of<GAME::Bullet>(destroyEntity))
			{
				auto playerView = registry.view<GAME::Player, GAME::HasBullet>();
				for (auto playerEntity : playerView)
				{
					registry.remove<GAME::HasBullet>(playerEntity);
				}
			}

			if (registry.any_of<DRAW::MeshCollection>(destroyEntity))
			{
				auto& meshCollection = registry.get<DRAW::MeshCollection>(destroyEntity).meshes;
				for (auto meshEntity : meshCollection)
				{
					if (registry.valid(meshEntity))
						registry.destroy(meshEntity);
				}
			}

			if (registry.any_of<GAME::PointValue>(destroyEntity))
			{
				const bool isEgg = registry.any_of<GAME::EggSac>(destroyEntity);

				if (!isEgg || registry.any_of<GAME::KilledByPlayer>(destroyEntity))
				{
					registry.ctx().get<GAME::Scoreboard>().score +=
						registry.get<GAME::PointValue>(destroyEntity).value;
				}
			}

			registry.destroy(destroyEntity);
		}

		//		-------------------if game over by enemies readching base ------------------------
		
		auto invasionView = registry.view<GAME::Enemy, GAME::Transform>();
		float loseZLine = -28.0f;

		for (auto enemyEntity : invasionView)
		{
			auto& transform = registry.get<GAME::Transform>(enemyEntity);

			if (transform.matrix.row4.z <= loseZLine)
			{
				if (registry.view<GAME::GameOver>().empty())
				{
					auto gameOverEntity = registry.create();
					registry.emplace<GAME::GameOver>(gameOverEntity);
					std::cout << "The spiders reached the base! Game Over." << std::endl;
				}
				break;
			}
		}

		auto enemyView = registry.view<GAME::Enemy>();

		if (enemyView.begin() == enemyView.end())
		{
			if (!registry.view<GAME::GameOver>().empty())
			{
				return;
			}

			if (!registry.any_of<GAME::LevelComplete>(entity))
			{
				registry.emplace<GAME::LevelComplete>(entity);
				std::cout << "Wave Cleared!" << std::endl;
			}
		}
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<Update_GameManager>();
	}
}
