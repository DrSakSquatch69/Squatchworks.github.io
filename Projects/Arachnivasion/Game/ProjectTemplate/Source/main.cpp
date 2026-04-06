// main entry point for the application
// enables components to define their behaviors locally in an .hpp file
#include "CCL.h"
#include "UTIL/Utilities.h"
// include all components, tags, and systems used by this program
#include "DRAW/DrawComponents.h"
#include "GAME/GameComponents.h"
#include "APP/Window.hpp"
#include "GAME/SpawnHelpers.h"
#include "APP/imgui.h"
#include "APP/imgui_impl_vulkan.h"
#include "APP/imgui_impl_win32.h"
#include "GAME/HighScore.h"
#include "GAME/NetworkHighscores.h"

#ifdef _WIN32
#include <Windows.h>

// Forward Win32 messages to ImGui so it can receive keyboard input
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#ifdef _WIN32
static WNDPROC g_OldWndProc = nullptr;
#endif

// credits UI state + loader
struct CreditsUIState {
	std::vector<std::string> lines;
	float offsetY = 0.0f;     // current Y offset for scrolling (pixels)
	float speed = 40.0f;      // pixels per second
	bool active = false;
	float totalHeight = 0.0f; // approximated total height in pixels
};

// Stores temporary UI state for entering high score initials.
// This lives in registry context so it persists across frames.
struct HighscoreEntryUI
{
	char initials[4] = ""; // 3 characters + null terminator
	bool submitted = false;   // prevents duplicate submissions
};

static void LoadCredits(entt::registry& registry, const char* path = "credits.txt")
{
	// ensure context entry exists
	if (!registry.ctx().contains<CreditsUIState>())
		registry.ctx().emplace<CreditsUIState>();
	auto& credits = registry.ctx().get<CreditsUIState>();

	credits.lines.clear();

	std::ifstream file(path);
	if (file) {
		std::string line;
		while (std::getline(file, line)) {
			// trim carriage return if present
			if (!line.empty() && line.back() == '\r') line.pop_back();
			credits.lines.push_back(line);
		}
	}
	else {
		// fallback default credits
		credits.lines = {
	"Developed by Nightshade Games",
	"",
	"Developers:",
	"Jack Jowers, Sara Fielders",
	"Jacob Blackburn, William Roland",
	"",
	"Music: Jack Jowers",
	"",
	"Thank you for playing!"
		};
	}

	// approximate total height (font size ~18 line step ~22-24)
	const float lineStep = 22.0f;
	credits.totalHeight = credits.lines.size() * lineStep;
	credits.offsetY = 0.0f;
	credits.active = false;
}

// Local routines for specific application behavior
void GraphicsBehavior(entt::registry& registry);
void GameplayBehavior(entt::registry& registry);
void MainLoopBehavior(entt::registry& registry);
static void ShutdownImGui(entt::registry& registry);

// highscores file
static const char* kHighscoreFile = "saved_highscores.txt";

// Architecture is based on components/entities pushing updates to other components/entities (via "patch" function)
int main()
{

	// All components, tags, and systems are stored in a single registry
	entt::registry registry;	

	// initialize the ECS Component Logic
	CCL::InitializeComponentLogic(registry);

	// Seed the rand
	unsigned int time = std::chrono::steady_clock::now().time_since_epoch().count();
	srand(time);

	registry.ctx().emplace<UTIL::Config>();
	registry.ctx().emplace<GAME::GameState>();
	registry.ctx().emplace<GAME::Scoreboard>();
	registry.ctx().get<GAME::Scoreboard>().score = 0;
	GAME::EnsureHighscoresLoaded(registry, kHighscoreFile);
	registry.ctx().emplace<GAME::SplashState>();
	registry.ctx().get<GAME::GameState>().state = GAME::State::Splash;

	// Create high score UI state used during the Game Over screen
	registry.ctx().emplace<HighscoreEntryUI>();

	// create/load credits
	if (!registry.ctx().contains<CreditsUIState>())
		registry.ctx().emplace<CreditsUIState>();
	LoadCredits(registry, "credits.txt");

	GraphicsBehavior(registry); // create windows, surfaces, and renderers
	
	MainLoopBehavior(registry); // update windows and input

	ShutdownImGui(registry);
	auto responderView = registry.view<GW::CORE::GEventResponder>();
	for (auto entity : responderView) {
		registry.destroy(entity);
	}


	// clear all entities and components from the registry
	// invokes on_destroy() for all components that have it
	// registry will still be intact while this is happening
	registry.clear(); 

	return 0; // now destructors will be called for all components
}

// Returns absolute path to the local highscore file (next to the exe)
static std::string GetLocalHighscorePath()
{
	return (GAME::ExeDir() / "saved_highscores.txt").string();
}

static void UnloadLevel(entt::registry& registry, entt::entity displayEntity)
{
	if (registry.any_of<DRAW::VulkanVertexBuffer>(displayEntity))
		registry.remove<DRAW::VulkanVertexBuffer>(displayEntity);

	if (registry.any_of<DRAW::VulkanIndexBuffer>(displayEntity))
		registry.remove<DRAW::VulkanIndexBuffer>(displayEntity);

	if (registry.any_of<std::vector<H2B::VERTEX>>(displayEntity))
		registry.remove<std::vector<H2B::VERTEX>>(displayEntity);

	if (registry.any_of<std::vector<unsigned int>>(displayEntity))
		registry.remove<std::vector<unsigned int>>(displayEntity);

	if (registry.any_of<DRAW::GPULevel>(displayEntity))
		registry.remove<DRAW::GPULevel>(displayEntity);

	if (registry.any_of<DRAW::CPULevel>(displayEntity))
		registry.remove<DRAW::CPULevel>(displayEntity);
}

static VkDescriptorPool CreateImGuiPool(VkDevice device);

// ---- Imgui initialization ----
static void InitImGui(entt::registry& registry, entt::entity display)
{
	auto& ui = registry.ctx().get<GAME::ImGuiState>();
	if (ui.initialized) return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	// Load a Unicode-capable font (Windows default)
	io.Fonts->AddFontFromFileTTF(
		"C:/Windows/Fonts/segoeui.ttf",
		18.0f,
		nullptr,
		io.Fonts->GetGlyphRangesDefault()
	);

	// splash logo font
	ui.splashFont = io.Fonts->AddFontFromFileTTF(
		"../Models/Tiny5-Regular.ttf",
		64.0f
	);
	auto& gwWin = registry.get<GW::SYSTEM::GWindow>(display);
	GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh{};
	gwWin.GetWindowHandle(uwh);
	HWND hwnd = (HWND)uwh.window;

	ImGui_ImplWin32_Init(hwnd);

#ifdef _WIN32
	// ------------------------------------------------------------
	// Hook Win32 WndProc so ImGui receives keyboard input
	// ------------------------------------------------------------
	g_OldWndProc = (WNDPROC)SetWindowLongPtr(
		hwnd,
		GWLP_WNDPROC,
		(LONG_PTR)+[](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			// Let ImGui process keyboard & text input
			if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
				return true;

			// Forward message to original window procedure
			return CallWindowProc(g_OldWndProc, hWnd, msg, wParam, lParam);
		});
#endif
		auto& surface = registry.get<DRAW::VulkanRenderer>(display).vlkSurface;

	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice phys = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
	uint32_t gfxFamily = 0, presentFamily = 0;

	surface.GetQueueFamilyIndices(gfxFamily, presentFamily);
	surface.GetInstance((void**)&instance);
	surface.GetPhysicalDevice((void**)&phys);
	surface.GetDevice((void**)&device);
	surface.GetRenderPass((void**)&renderPass);
	surface.GetGraphicsQueue((void**)&queue);
	ui.device = device;
	uint32_t imageCount = 2;
	surface.GetSwapchainImageCount(imageCount);

	ui.descriptorPool = CreateImGuiPool(device);

	ImGui_ImplVulkan_InitInfo init_info{};
	init_info.Instance = instance;
	init_info.PhysicalDevice = phys;
	init_info.Device = device;
	init_info.QueueFamily = gfxFamily;
	init_info.Queue = queue;
	init_info.DescriptorPool = ui.descriptorPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = imageCount;
	init_info.PipelineInfoMain.RenderPass = renderPass;
	init_info.PipelineInfoMain.Subpass = 0;

	ImGui_ImplVulkan_Init(&init_info);

	ui.initialized = true;
}

// --- cleaning ----
static void ShutdownImGui(entt::registry& registry)
{
	
	if (!registry.ctx().contains<GAME::ImGuiState>()) return;
	auto& ui = registry.ctx().get<GAME::ImGuiState>();
	if (!ui.initialized) return;

	if (ui.device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(ui.device);
	}

	// Restore original Win32 WndProc
	if (ui.hwnd && ui.oldWndProc)
	{
		SetWindowLongPtr(ui.hwnd, GWLP_WNDPROC, (LONG_PTR)ui.oldWndProc);
		ui.oldWndProc = nullptr;
		ui.hwnd = nullptr;
	}

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplWin32_Shutdown();

	if (ImGui::GetCurrentContext()) {
		ImGui::DestroyContext();
	}

	if (ui.device != VK_NULL_HANDLE) {
		if (ui.descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(ui.device, ui.descriptorPool, nullptr);
			ui.descriptorPool = VK_NULL_HANDLE;
		}
		ui.device = VK_NULL_HANDLE;
	}

	ui.initialized = false;
}

// This function will be called by the main loop to update the graphics
// It will be responsible for loading the Level, creating the VulkanRenderer, and all VulkanInstances
void GraphicsBehavior(entt::registry& registry)
{
	std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

	// ------ audio loads ------

	auto& bank = registry.ctx().emplace<GAME::AudioBank>();

	bank.enabled = (*config).at("Audio").at("enabled").as<int>() != 0;
	bank.master = (*config).at("Audio").at("master").as<float>();

	// read optional config-controlled music/sfx multipliers (fallbacks)
	if ((*config).find("Music") != (*config).end()) {
		const auto& musicSection = (*config).at("Music");
		if (musicSection.find("globalMusicVolume") != musicSection.end())
			bank.musicVolume = musicSection.at("globalMusicVolume").as<float>();
	}
	// no global SFX entry in defaults.ini by default; fallback to 1.0
	bank.sfxVolume = 1.0f;

	bank.audio.Create();

	auto LoadSFX = [&](GW::AUDIO::GSound& s, const char* key, const char* volKey)
		{
			if (!bank.enabled) return;

			std::string rel = (*config).at("SFX").at(key).as<std::string>();
			float vol = (*config).at("SFX").at(volKey).as<float>();

			std::filesystem::path abs =
				std::filesystem::weakly_canonical(std::filesystem::current_path() / rel);

			bool exists = std::filesystem::exists(abs);

			std::ifstream test(abs, std::ios::binary);
			bool canOpen = (bool)test;

			std::cout << "[AUDIO] " << key
				<< " rel=" << rel
				<< " abs=" << abs.string()
				<< " exists=" << exists
				<< " open=" << canOpen;

			if (exists) {
				std::error_code ec;
				auto sz = std::filesystem::file_size(abs, ec);
				if (!ec) std::cout << " size=" << sz;
			}
			std::cout << "\n";

			auto r = s.Create(rel.c_str(), bank.audio, vol);
			std::cout << "[AUDIO] Create(" << key << ") -> " << (int)r << " vol=" << vol << "\n";
		};

	auto LoadMusic = [&](GW::AUDIO::GMusic& m,
		const char* section,
		const char* pathKey,
		const char* volKey,
		const char* loopKey,
		const char* playKey,
		bool& outLoaded,
		bool& outLoop)
		{
			if (!bank.enabled) return;

			std::string rel = (*config).at(section).at(pathKey).as<std::string>();
			float vol = (*config).at(section).at(volKey).as<float>();
			bool loop = (*config).at(section).at(loopKey).as<bool>();
			bool playOnStart = (*config).at(section).at(playKey).as<bool>();

			std::filesystem::path abs =
				std::filesystem::weakly_canonical(std::filesystem::current_path() / rel);

			bool exists = std::filesystem::exists(abs);
			std::ifstream test(abs, std::ios::binary);
			bool canOpen = (bool)test;

			std::cout << "[AUDIO] MUSIC " << section
				<< " rel=" << rel
				<< " abs=" << abs.string()
				<< " exists=" << exists
				<< " open=" << canOpen;

			if (exists) {
				std::error_code ec;
				auto sz = std::filesystem::file_size(abs, ec);
				if (!ec) std::cout << " size=" << sz;
			}
			std::cout << "\n";

			auto r = m.Create(rel.c_str(), bank.audio, vol);
			std::cout << "[AUDIO] GMusic::Create(" << section << ") -> " << (int)r << " vol=" << vol << "\n";

			outLoaded = (r == GW::GReturn::SUCCESS);
			outLoop = loop;

			if (outLoaded && playOnStart) {
				auto pr = m.Play(loop);
				std::cout << "[AUDIO] GMusic::Play(" << section << ", loop=" << loop << ") -> " << (int)pr << "\n";
			}
		};

	LoadMusic(bank.bgm, "Music", "path", "volume", "loop", "playOnStart",
		bank.bgmLoaded, bank.bgmLoop);
	LoadMusic(bank.menuMusic, "Music.MainMenu", "path", "volume", "loop", "playOnStart",
		bank.menuLoaded, bank.menuLoop);
	LoadSFX(bank.shoot, "shoot", "shootVol");
	LoadSFX(bank.enemyDie, "enemyDie", "enemyDieVol");
	LoadSFX(bank.eggSpawn, "eggSpawn", "eggSpawnVol");
	LoadSFX(bank.eggDie, "eggDie", "eggDieVol");

	// Add an entity to handle all the graphics data
	auto display = registry.create();

	// TODO: Emplace CPULevel. Placing here to reduce occurrence of a json race condition crash
	//std::string levelPath = (*config).at("Level1").at("levelFile").as<std::string>();
	//std::string modelPath = (*config).at("Level1").at("modelPath").as<std::string>();

	//registry.emplace<DRAW::CPULevel>(display, DRAW::CPULevel{ levelPath, modelPath });

	// Emplace and initialize Window component
	int windowWidth = (*config).at("Window").at("width").as<int>();
	int windowHeight = (*config).at("Window").at("height").as<int>();
	int startX = (*config).at("Window").at("xstart").as<int>();
	int startY = (*config).at("Window").at("ystart").as<int>();
	registry.emplace<APP::Window>(display,
		APP::Window{ startX, startY, windowWidth, windowHeight, GW::SYSTEM::GWindowStyle::WINDOWEDLOCKED, "Arachnivasion by Nightshade Games"});


	// Create the input
	auto& input =  registry.ctx().emplace<UTIL::Input>();
	auto& window = registry.get<GW::SYSTEM::GWindow>(display);
	input.bufferedInput.Create(window);
	input.immediateInput.Create(window);
	input.gamePads.Create();
	auto& pressEvents = registry.ctx().emplace<GW::CORE::GEventCache>();
	pressEvents.Create(32);
	input.bufferedInput.Register(pressEvents);
	input.gamePads.Register(pressEvents);

	// Create a transient component to initialize the Renderer
	std::string vertShader = (*config).at("Shaders").at("vertex").as<std::string>();
	std::string pixelShader = (*config).at("Shaders").at("pixel").as<std::string>();
	registry.emplace<DRAW::VulkanRendererInitialization>(display,
		DRAW::VulkanRendererInitialization{ 
			vertShader, pixelShader,
			{ {0.2f, 0.2f, 0.25f, 1} } , { 1.0f, 0u }, 75.f, 0.1f, 100.0f });
	registry.emplace<DRAW::VulkanRenderer>(display);
	
	// TODO : Emplace GPULevel
	
//	registry.emplace<DRAW::GPULevel>(display);

	// Register for Vulkan clean up
	GW::CORE::GEventResponder shutdown;
	shutdown.Create([&](const GW::GEvent& e) {
		GW::GRAPHICS::GVulkanSurface::Events event;
		GW::GRAPHICS::GVulkanSurface::EVENT_DATA data;
		if (+e.Read(event, data) && event == GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES) {
			ShutdownImGui(registry);
			registry.clear<DRAW::VulkanRenderer>();
		}
		});
	registry.get<DRAW::VulkanRenderer>(display).vlkSurface.Register(shutdown);
	registry.emplace<GW::CORE::GEventResponder>(display, shutdown.Relinquish());

	registry.ctx().emplace<GAME::ImGuiState>();
	InitImGui(registry, display);

	// Create a camera and emplace it
	GW::MATH::GMATRIXF initialCamera;
	GW::MATH::GMatrix::IdentityF(initialCamera);
	GW::MATH::GVECTORF translate = { 0.0f,  45.0f, -5.0f };
	GW::MATH::GVECTORF lookat = { 0.0f, 0.0f, 0.0f };
	GW::MATH::GVECTORF up = { 0.0f, 1.0f, 0.0f };
	GW::MATH::GMatrix::TranslateGlobalF(initialCamera, translate, initialCamera);
	GW::MATH::GMatrix::LookAtLHF(translate, lookat, up, initialCamera);
	// Inverse to turn it into a camera matrix, not a view matrix. This will let us do
	// camera manipulation in the component easier
	GW::MATH::GMatrix::InverseF(initialCamera, initialCamera);
	registry.emplace<DRAW::Camera>(display,
		DRAW::Camera{ initialCamera });

	GAME::LoadGlobalHighscoresFromServer(registry);
}

// ---- Imgui setup ----
static VkDescriptorPool CreateImGuiPool(VkDevice device)
{
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
	};

	VkDescriptorPoolCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	info.maxSets = 1000 * (uint32_t)(sizeof(pool_sizes) / sizeof(pool_sizes[0]));
	info.poolSizeCount = (uint32_t)(sizeof(pool_sizes) / sizeof(pool_sizes[0]));
	info.pPoolSizes = pool_sizes;

	VkDescriptorPool pool = VK_NULL_HANDLE;
	vkCreateDescriptorPool(device, &info, nullptr, &pool);
	return pool;
}

static GW::MATH::GVECTORF GetEnemyRowColor(
	const ini::IniFile& cfg,
	int rowIndex)
{
	const std::string section = "EnemyColors";
	const std::string key = "row" + std::to_string(rowIndex);

	if (cfg.find(section) == cfg.end())
		return { 1.0f, 1.0f, 1.0f }; // fallback: white

	const auto& colors = cfg.at(section);

	if (colors.find(key) == colors.end())
		return { 1.0f, 1.0f, 1.0f };

	std::string value = colors.at(key).as<std::string>();

	float r = 1.0f, g = 1.0f, b = 1.0f;
	sscanf_s(value.c_str(), "%f,%f,%f", &r, &g, &b);

	return { r, g, b };
}

static int GetEnemyRowPoints(const ini::IniFile& cfg, int rowIndex)
{
	const std::string section = "EnemyPoints";
	const std::string key = "row" + std::to_string(rowIndex);

	if (cfg.find(section) == cfg.end())
		return 10;

	const auto& pts = cfg.at(section);
	if (pts.find(key) == pts.end())
		return 10;

	return pts.at(key).as<int>();
}

// This function will be called by the main loop to update the gameplay
// It will be responsible for updating the VulkanInstances and any other gameplay components
void GameplayBehavior(entt::registry& registry)
{
	std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

	// this gets our current level.
	int currentLevelNumber = 1;
	auto levelView = registry.view<GAME::CurrentLevel>();

	if (!levelView.empty()) {
		auto entity = levelView.front();
		currentLevelNumber = registry.get<GAME::CurrentLevel>(entity).number;
	}
	else {
		auto entity = registry.create();
		registry.emplace<GAME::CurrentLevel>(entity, GAME::CurrentLevel{ 1 });
	}

	int waveNumber = currentLevelNumber;
	const std::string levelSection = "Level1";
	int baseEnemyCount = (*config).at(levelSection).at("enemyCount").as<int>();

	std::string playerModelName = (*config).at("Player").at("model").as<std::string>();
	int maxHealth = (*config).at("Player").at("hitpoints").as<int>();
	int lives = (*config).at("Player").at("lives").as<int>();

	auto ctxView = registry.view<GW::SYSTEM::GWindow>();
	entt::entity ctxEntity = ctxView.front();

	auto& modelManager = registry.get<DRAW::ModelManager>(ctxEntity);

	// ------------------------------------------------------------
	// Create player ONLY if one does not already exist.
	// This allows health and lives to persist across waves.
	// ------------------------------------------------------------
	entt::entity player = entt::null;

	auto playerView = registry.view<GAME::Player>();

	if (playerView.empty())
	{
		// First time spawning the player
		player = registry.create();

		registry.emplace<GAME::Player>(player);
		registry.emplace<GAME::Health>(player, GAME::Health{ maxHealth });
		registry.emplace<GAME::Lives>(player, GAME::Lives{ lives });

		SpawnGameEntity(registry, modelManager, playerModelName, player);
	}
	else
	{
		// Player already exists — reuse it
		player = *playerView.begin();
	}

	// ---------------- start of enemy spawning information ----------------
	std::string enemyModelName = (*config).at("Enemy1").at("model").as<std::string>();
	float enemySpeed = (*config).at("Enemy1").at("speed").as<float>();
	int enemyHealth = (*config).at("Enemy1").at("hitpoints").as<int>();

	int rows = (*config).at("Enemy1").at("rows").as<int>();
	int cols = (*config).at("Enemy1").at("cols").as<int>();

	float spacingX = (*config).at("Enemy1").at("spacingX").as<float>();
	float spacingZ = (*config).at("Enemy1").at("spacingZ").as<float>();

	float startX = (*config).at("Enemy1").at("startX").as<float>();
	float startZ = (*config).at("Enemy1").at("startZ").as<float>();

	float enemyScale = (*config).at("Enemy1").at("scale").as<float>();

	float gridWidth = (cols - 1) * spacingX;
	float leftX = startX - (gridWidth * 0.5f);

	// ------------ difficultly scaling --------------

	// wave-based starting pos 
	//int waveNumber = 1;
	if (!levelView.empty())
		waveNumber = registry.get<GAME::CurrentLevel>(levelView.front()).number;

	float waveDropPerWave = (*config).at("Enemy1").at("waveDropPerWave").as<float>();
	int waveResetWave = (*config).at("Enemy1").at("waveResetWave").as<int>();

	int cycleSteps = waveResetWave - 1;
	int stepIndex = 0;

	if (cycleSteps > 0)
		stepIndex = (waveNumber - 1) % cycleSteps;

	float waveStartZ = startZ - (stepIndex * waveDropPerWave);

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			entt::entity enemy = registry.create();
			registry.emplace<GAME::Enemy>(enemy);
			registry.emplace<GAME::Health>(enemy, GAME::Health{ enemyHealth });

			// movement
			GW::MATH::GVECTORF dir = { 1.0f, 0.0f, 0.0f, 0.0f };
			registry.emplace<GAME::Velocity>(enemy, GAME::Velocity{ dir, enemySpeed });

			// transform
			GW::MATH::GMATRIXF m;
			GW::MATH::GMatrix::IdentityF(m);

			// scale (this is the "fit on screen" control)
			GW::MATH::GVECTORF s = { enemyScale, enemyScale, enemyScale, 0.0f };
			GW::MATH::GMatrix::ScaleGlobalF(m, s, m);

			// grid position
			m.row4.x = leftX + c * spacingX;
			m.row4.y = 0.0f;
			m.row4.z = waveStartZ - r * spacingZ;

			registry.emplace<GAME::Transform>(enemy, GAME::Transform{ m });
			// points
			int pointsForThisRow = GetEnemyRowPoints(*config, r);
			registry.emplace<GAME::PointValue>(enemy, GAME::PointValue{ pointsForThisRow });

			SpawnGameEntity(registry, modelManager, enemyModelName, enemy);
			// ---------------- apply row-based color (data-driven) ----------------
			{
				// pull color from defaults.ini
				GW::MATH::GVECTORF tint =
					GetEnemyRowColor(*config, r);

				auto& meshCollection = registry.get<DRAW::MeshCollection>(enemy);

				for (auto meshEntity : meshCollection.meshes)
				{
					registry.patch<DRAW::GPUInstance>(meshEntity,
						[&](DRAW::GPUInstance& inst)
						{
							inst.matData.Kd = { tint.x, tint.y, tint.z };
						});
				}
			}
		}
	}

	auto gameManager = registry.create();
	registry.emplace<GAME::GameManager>(gameManager);
	registry.emplace<GAME::EnemyFireController>(gameManager, GAME::EnemyFireController{});
	auto& spawner = registry.emplace<GAME::EggSacSpawner>(gameManager, GAME::EggSacSpawner{});

	auto& cfg = *registry.ctx().get<UTIL::Config>().gameConfig;
	if (cfg.find("EggSac") != cfg.end())
	{
		auto& eggCfg = cfg.at("EggSac");

		float intervalMin = eggCfg.at("spawnIntervalMin").as<float>();
		float intervalMax = eggCfg.at("spawnIntervalMax").as<float>();

		auto Rand01 = []() -> float { return (float)rand() / (float)RAND_MAX; };
		auto RandRange = [&](float a, float b) -> float { return a + (b - a) * Rand01(); };

		spawner.timer = RandRange(intervalMin, intervalMax);
		spawner.active = true;
	}
	else
	{
		spawner.timer = 10.0f;
		spawner.active = true;
	}

	// this will control the enemy swarm speed ( its just the enemy speed btw)
	registry.emplace<GAME::Swarm>(gameManager, GAME::Swarm{ enemySpeed });

	// ------------ bunker information --------------
	
	std::string bunkermodel = (*config).at("Bunker").at("model").as<std::string>();
	float bScaleX = (*config).at("Bunker").at("scalex").as<float>();
	float bScaleZ = (*config).at("Bunker").at("scalez").as<float>();
	int bHealth = (*config).at("Bunker").at("hitpoints").as<int>();

	// Needs less hardcoding >:l
	for (int i = 21; i >= -21; i -= 14)
	{
		for (int x = 0; x <= 6; x += 2)
		{
			for (int z = 0; z <= 4; z += 2)
			{
				entt::entity bunker = registry.create();
				registry.emplace<GAME::Bunker>(bunker);
				registry.emplace<GAME::Health>(bunker, GAME::Health{ bHealth });


				GW::MATH::GMATRIXF bm;
				GW::MATH::GMatrix::IdentityF(bm);

				GW::MATH::GVECTORF bv = { (float)(i + x), 1.0f, (float)(-18.0 - z) };
				GW::MATH::GMatrix::TranslateGlobalF(bm, bv, bm);

				registry.emplace<GAME::Transform>(bunker, GAME::Transform{ bm });
				SpawnGameEntity(registry, modelManager, bunkermodel, bunker);

			}
		}
	}
}

// --------------- state based music control ---------------
static void ApplyMusicForState(entt::registry& registry, GAME::State s)
{
	if (!registry.ctx().contains<GAME::AudioBank>()) return;
	auto& bank = registry.ctx().get<GAME::AudioBank>();
	if (!bank.enabled) return;

	auto StopAll = [&]()
		{
			if (bank.menuLoaded) bank.menuMusic.Stop();
			if (bank.bgmLoaded)  bank.bgm.Stop();
			bank.now = GAME::AudioBank::MusicNow::None;
			bank.musicPaused = false;
		};

	switch (s)
	{
	case GAME::State::GameOver:
		StopAll();
		break;

	case GAME::State::Paused:
		if (!bank.musicPaused) {
			if (bank.now == GAME::AudioBank::MusicNow::Menu && bank.menuLoaded)
				bank.menuMusic.Pause();
			else if (bank.now == GAME::AudioBank::MusicNow::Gameplay && bank.bgmLoaded)
				bank.bgm.Pause();

			bank.musicPaused = true;
		}
		break;

	case GAME::State::MainMenu:
		if (bank.bgmLoaded) bank.bgm.Stop();

		if (bank.menuLoaded) {
			if (bank.now != GAME::AudioBank::MusicNow::Menu) {
				bank.menuMusic.Play(bank.menuLoop);
				bank.now = GAME::AudioBank::MusicNow::Menu;
				bank.musicPaused = false;
			}
			else if (bank.musicPaused) {
				bank.menuMusic.Resume();
				bank.musicPaused = false;
			}
		}
		break;

	case GAME::State::Playing:
		if (bank.menuLoaded) bank.menuMusic.Stop();

		if (bank.bgmLoaded) {
			if (bank.now != GAME::AudioBank::MusicNow::Gameplay) {
				bank.bgm.Play(bank.bgmLoop);
				bank.now = GAME::AudioBank::MusicNow::Gameplay;
				bank.musicPaused = false;
			}
			else if (bank.musicPaused) {
				bank.bgm.Resume();
				bank.musicPaused = false;
			}
		}
		break;

	default:
		break;
	}
}

// This function will be called by the main loop to update the main loop
// It will be responsible for updating any created windows and handling any input
void MainLoopBehavior(entt::registry& registry)
{
	// main loop
	int closedCount; // count of closed windows
	auto winView = registry.view<APP::Window>(); // for updating all windows

	auto& inputWrapper = registry.ctx().get<UTIL::Input>();
	auto& input = inputWrapper.immediateInput;
	std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
	float playerFireRate = (*config).at("Player").at("firerate").as<float>();
	std::string bulletModelName = (*config).at("Bullet").at("model").as<std::string>();
	float bulletSpeed = (*config).at("Bullet").at("speed").as<float>();
	auto ctxView = registry.view<GW::SYSTEM::GWindow>();
	entt::entity ctxEntity = ctxView.front();
	//	auto& modelManager = registry.get<DRAW::ModelManager>(ctxEntity);

	auto& deltaTime = registry.ctx().emplace<UTIL::DeltaTime>().dtSec;
	// for updating all windows
	auto gameView = registry.view<GAME::GameManager>();
	do
	{

		// Set the delta time
		static auto start = std::chrono::steady_clock::now();
		double elapsed = std::chrono::duration<double>(
			std::chrono::steady_clock::now() - start).count();
		start = std::chrono::steady_clock::now();
		// Cap delta time to min 30 fps. This will prevent too much time from simulating when dragging the window
		if (elapsed > 1.0 / 30.0)
		{
			elapsed = 1.0 / 30.0;
		}
		deltaTime = elapsed;

		float dt = static_cast<float>(deltaTime);



		//Pause menu
		auto& gameState = registry.ctx().get<GAME::GameState>();
		// Block gameplay hotkeys while ImGui is typing/capturing keyboard
		bool blockHotkeys = false;
		if (registry.ctx().contains<GAME::ImGuiState>()) {
			auto& imguiState = registry.ctx().get<GAME::ImGuiState>();
			if (imguiState.initialized && ImGui::GetCurrentContext() != nullptr) {
				ImGuiIO& io = ImGui::GetIO();
				blockHotkeys = (io.WantCaptureKeyboard || io.WantTextInput);
			}
		}
		static bool escWasDown = false;
		float esc = 0.0f;
		input.GetState(G_KEY_ESCAPE, esc);
		bool escDown = (esc != 0.0f);
		if (!blockHotkeys && escDown && !escWasDown)
		{
			//only toggle when currently played or paused
			if (gameState.state == GAME::State::Playing)
			{
				gameState.state = GAME::State::Paused;
				std::cout << "Game Paused\n";
			}
			else if (gameState.state == GAME::State::Paused)
			{
				gameState.state = GAME::State::Playing;
				std::cout << "Game Resumed\n";
			}
		}
		escWasDown = escDown;

		//End Pause Menu

		static bool savedThisGameOver = false;

		bool shouldBeGameOver =
			(gameState.state != GAME::State::MainMenu) &&
			!registry.view<GAME::GameOver>().empty();

		if (shouldBeGameOver)
		{
			if (gameState.state != GAME::State::GameOver)
				savedThisGameOver = false;

			gameState.state = GAME::State::GameOver;

			if (!savedThisGameOver)
			{
				// High scores are submitted through the ImGui Game Over screen.
				// Do NOT auto-save here to avoid duplicate or invalid entries.
			}
		}
		else
		{
			savedThisGameOver = false;
		}

		// changes music 
		static GAME::State prevMusicState = (GAME::State)-1;
		if (gameState.state != prevMusicState)
		{
			ApplyMusicForState(registry, gameState.state);
			prevMusicState = gameState.state;
		}

		// ------------ Splash screen ------------

		auto& gs = registry.ctx().get<GAME::GameState>();
		auto& splash = registry.ctx().get<GAME::SplashState>();

		if (registry.ctx().contains<UTIL::DeltaTime>())
			dt = (float)registry.ctx().get<UTIL::DeltaTime>().dtSec;

		if (gs.state == GAME::State::Splash) {
			splash.started = true;
			splash.t += dt;

			float total = splash.fadeIn + splash.hold + splash.fadeOut;
			if (splash.t >= total)
			{
				splash.t = 0.0f;
				splash.index++;

				if (splash.index >= (int)splash.screens.size())
				{
					gs.state = GAME::State::MainMenu;
				}
			}

			goto EndOfFrame;
		}


		// ---------------------- main menu ----------------------


		if (gameState.state == GAME::State::MainMenu)
		{
			static bool enterWasDown = false;

			float enter = 0.0f;
			input.GetState(G_KEY_ENTER, enter);
			bool enterDown = (enter != 0.0f);

			if (enterDown && !enterWasDown)
			{
				auto vrView = registry.view<DRAW::VulkanRenderer>();
				if (!vrView.empty())
				{
					auto displayEntity = vrView.front();

					std::string levelPath = (*config).at("Level1").at("levelFile").as<std::string>();
					std::string modelPath = (*config).at("Level1").at("modelPath").as<std::string>();

					if (registry.any_of<DRAW::GPULevel>(displayEntity))
						registry.remove<DRAW::GPULevel>(displayEntity);

					if (registry.any_of<DRAW::CPULevel>(displayEntity))
						registry.remove<DRAW::CPULevel>(displayEntity);

					registry.emplace<DRAW::CPULevel>(displayEntity, DRAW::CPULevel{ levelPath, modelPath });
					registry.emplace<DRAW::GPULevel>(displayEntity);

					GameplayBehavior(registry);

					gameState.state = GAME::State::Playing;
				}
			}

			enterWasDown = enterDown;
			goto EndOfFrame;
		}
		if (gameState.state == GAME::State::Paused)
		{
			goto EndOfFrame;
		}
		if (gameState.state == GAME::State::Playing || gameState.state == GAME::State::GameOver)
		{

			// ---------------------- Level complete ----------------------
			auto levelCompleteView = registry.view<GAME::LevelComplete>();
			if (!levelCompleteView.empty())
			{
				auto entity = levelCompleteView.front();
				registry.remove<GAME::LevelComplete>(entity);

				auto levelView = registry.view<GAME::CurrentLevel>();
				if (!levelView.empty()) {
					registry.get<GAME::CurrentLevel>(levelView.front()).number++;
				}

				auto SafeDestroy = [&](entt::entity e) {
					if (registry.any_of<DRAW::MeshCollection>(e)) {
						auto& meshes = registry.get<DRAW::MeshCollection>(e).meshes;
						for (auto mesh : meshes) {
							if (registry.valid(mesh)) registry.destroy(mesh);
						}
					}
					if (registry.valid(e)) registry.destroy(e);
					};

				std::vector<entt::entity> toDestroy;

				auto enemies = registry.view<GAME::Enemy>();
				for (auto e : enemies) toDestroy.push_back(e);

				auto bullets = registry.view<GAME::Bullet>();
				for (auto e : bullets) toDestroy.push_back(e);

				auto enemybullets = registry.view<GAME::EnemyBullet>();
				for (auto e : enemybullets) toDestroy.push_back(e);

				auto bunkers = registry.view<GAME::Bunker>();
				for (auto e : bunkers) toDestroy.push_back(e);

				auto managers = registry.view<GAME::GameManager>();
				for (auto e : managers) toDestroy.push_back(e);

				for (auto e : toDestroy) SafeDestroy(e);

				// OPTIONAL: clear per-wave player state
				for (auto p : registry.view<GAME::Player>()) {
					if (registry.any_of<GAME::HasBullet>(p)) registry.remove<GAME::HasBullet>(p);
					if (registry.any_of<GAME::Firing>(p)) registry.remove<GAME::Firing>(p);
					if (registry.any_of<GAME::Invulnerability>(p)) registry.remove<GAME::Invulnerability>(p);
					if (registry.any_of<GAME::GameOver>(p)) registry.remove<GAME::GameOver>(p);
				}
				// Optional: reposition player at new wave start WITHOUT touching HP/Lives
				for (auto p : registry.view<GAME::Player, GAME::Transform>()) {
					auto& t = registry.get<GAME::Transform>(p);
					t.matrix.row4.x = 0.0f;
					t.matrix.row4.z = -28.0f;
				}

				GameplayBehavior(registry);
				gameState.state = GAME::State::Playing;
			}

			//		---------------------------- restart game ---------------------------
			auto gameOverView = registry.view<GAME::GameOver>();
			if (!gameOverView.empty())
			{
				if (!blockHotkeys)
				{
					float restart = 0.0f;
					input.GetState(G_KEY_R, restart);

					if (restart != 0.0f)
					{
						auto SafeDestroy = [&](entt::entity e) {
							if (registry.any_of<DRAW::MeshCollection>(e)) {
								auto& meshes = registry.get<DRAW::MeshCollection>(e).meshes;
								for (auto mesh : meshes) {
									if (registry.valid(mesh)) registry.destroy(mesh);
								}
							}
							if (registry.valid(e)) registry.destroy(e);
						};

						std::vector<entt::entity> toReset;

						auto players = registry.view<GAME::Player>();
						for (auto e : players) toReset.push_back(e);

						auto enemies = registry.view<GAME::Enemy>();
						for (auto e : enemies) toReset.push_back(e);

						auto bullets = registry.view<GAME::Bullet>();
						for (auto e : bullets) toReset.push_back(e);

						auto enemybullets = registry.view<GAME::EnemyBullet>();
						for (auto e : enemybullets) toReset.push_back(e);

						auto bunkers = registry.view<GAME::Bunker>();
						for (auto e : bunkers) toReset.push_back(e);

						auto managers = registry.view<GAME::GameManager>();
						for (auto e : managers) toReset.push_back(e);

						for (auto e : toReset) SafeDestroy(e);
							
						auto levelView = registry.view<GAME::CurrentLevel>();
						if (!levelView.empty()) {
							registry.get<GAME::CurrentLevel>(levelView.front()).number = 1;
						}

						registry.clear<GAME::GameOver>();

						GameplayBehavior(registry);

						gameState.state = GAME::State::Playing;
					}
				}
			}

			// ---------------------------

			// invuln decrement timer
			auto invView = registry.view<GAME::Invulnerability>();
			for (auto e : invView)
			{
				auto& inv = invView.get<GAME::Invulnerability>(e);
				inv.cooldown -= dt;

				if (inv.cooldown <= 0.0f)
					registry.remove<GAME::Invulnerability>(e);
			}

			if (gameState.state != GAME::State::GameOver)
			{
				auto moveView = registry.view<GAME::Transform, GAME::Velocity>();
				moveView.each([dt](auto& transform, auto& velocity) {

					GW::MATH::GVECTORF translation;
					GW::MATH::GVector::ScaleF(velocity.direction, velocity.speed * dt, translation);
					GW::MATH::GMatrix::TranslateGlobalF(transform.matrix, translation, transform.matrix);
					});
			}
			// ---------------------- player firing ----------------------
			if (gameState.state == GAME::State::Playing)
			{

				auto playerView = registry.view<GAME::Player>();
				for (auto playerEntity : playerView)
				{
					if (registry.any_of<GAME::Firing>(playerEntity))
					{
						auto& firing = registry.get<GAME::Firing>(playerEntity);
						firing.cooldown -= dt;
						if (firing.cooldown <= 0.0f)
							registry.remove<GAME::Firing>(playerEntity);
					}

					if (registry.any_of<GAME::Invulnerability>(playerEntity))
					{
						auto& invuln = registry.get<GAME::Invulnerability>(playerEntity);
						invuln.cooldown -= dt;
						if (invuln.cooldown <= 0.0f)
							registry.remove<GAME::Invulnerability>(playerEntity);
					}
					if (!registry.any_of<GAME::HasBullet>(playerEntity) &&
						!registry.any_of<GAME::Firing>(playerEntity))
					{
						auto isKeyDown = [&](int key) {
							float res = 0.0f;
							input.GetState(key, res);
							return res != 0.0f;
							};

						float inputX = 0.0f;
						float inputZ = 0.0f;

						if (isKeyDown(G_KEY_UP)) inputZ += 1.0f;

						if (inputX != 0.0f || inputZ != 0.0f)
						{
							auto bullet = registry.create();
							registry.emplace<GAME::Bullet>(bullet);

							// copy player position
							if (registry.all_of<GAME::Transform>(playerEntity)) {
								const auto& pTrans = registry.get<GAME::Transform>(playerEntity);

								GW::MATH::GMATRIXF bulletMatrix;
								GW::MATH::GMatrix::IdentityF(bulletMatrix);
								bulletMatrix.row4 = pTrans.matrix.row4;

								registry.emplace<GAME::Transform>(bullet, GAME::Transform{ bulletMatrix });
							}

							GW::MATH::GVECTORF dir = { inputX, 0.0f, inputZ };
							GW::MATH::GVECTORF normDir;
							GW::MATH::GVector::NormalizeF(dir, normDir);

							registry.emplace<GAME::Velocity>(bullet, GAME::Velocity{ normDir, bulletSpeed });

							auto& modelManager = registry.get<DRAW::ModelManager>(ctxEntity);
							SpawnGameEntity(registry, modelManager, bulletModelName, bullet);

							registry.emplace<GAME::HasBullet>(playerEntity);

							if (registry.ctx().contains<GAME::AudioBank>()) {
								auto& bank = registry.ctx().get<GAME::AudioBank>();
								if (bank.enabled) bank.shoot.Play();
							}

							registry.emplace<GAME::Firing>(playerEntity, playerFireRate);
						}
					}
				}

			}
		}


		if (gameState.state == GAME::State::Playing || gameState.state == GAME::State::GameOver)
		{
			auto gameView = registry.view<GAME::GameManager>();
			for (auto entity : gameView) {
				registry.patch<GAME::GameManager>(entity);
			}
		}
	EndOfFrame:
		closedCount = 0;

		// ----- ImGui per-frame handling (run ONCE per frame, not per window) -----

		if (registry.ctx().contains<GAME::ImGuiState>()) {
			auto& imgui = registry.ctx().get<GAME::ImGuiState>();
			if (imgui.initialized) {

				// --- BEGIN: Feed native mouse state into ImGui so buttons receive clicks ---
				// Gateware may not forward native Win32 messages to ImGui_ImplWin32; poll and push here.
				{
					ImGuiIO& io = ImGui::GetIO();

					// Get HWND from the context window (ctxEntity exists above)
					GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh{};
					auto& gwWin = registry.get<GW::SYSTEM::GWindow>(ctxEntity);
					gwWin.GetWindowHandle(uwh);
					HWND hwnd = (HWND)uwh.window;

					POINT p;
					if (GetCursorPos(&p) && ScreenToClient(hwnd, &p)) {
						io.MousePos = ImVec2((float)p.x, (float)p.y);
					}
					else {
						io.MousePos = ImVec2(-1e6f, -1e6f);
					}

					io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
					io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
					io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

				}
				// --- END: Feed native mouse state into ImGui ---

				ImGui_ImplVulkan_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				// ---------------- PLAYER HUD (Health + Lives) ----------------
				{
					auto playerView = registry.view<GAME::Player, GAME::Health, GAME::Lives>();
					if (playerView.begin() != playerView.end())
					{
						auto player = playerView.front();
						int hp = registry.get<GAME::Health>(player).value;
						int lives = registry.get<GAME::Lives>(player).value;

						ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
						ImGui::SetNextWindowBgAlpha(0.35f);

						ImGuiWindowFlags flags =
							ImGuiWindowFlags_NoDecoration |
							ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoSavedSettings |
							ImGuiWindowFlags_NoInputs;

						ImGui::Begin("PlayerHUD", nullptr, flags);
						ImGui::Text("Health:");
						for (int i = 0; i < hp; ++i)
						{
							ImGui::SameLine();
							ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"♥");
						}
						ImGui::Text("Lives:  %d", lives);
						// score on hud
						int score = 0;
						if (registry.ctx().contains<GAME::Scoreboard>())
							score = registry.ctx().get<GAME::Scoreboard>().score;

						ImGui::Text("Score: %d", score);

						int highScore = 0;

						if (registry.ctx().contains<GAME::Highscores>())
						{
							const auto& hs = registry.ctx().get<GAME::Highscores>();

							// Ensure at least one score exists
							if (hs.count > 0)
								highScore = hs.entries[0].score;
						}

						ImGui::Text("High Score: %d", highScore);

						ImGui::End();

					}
				}
				// Helper lambdas that reuse the same destruction/creation logic you use elsewhere
				auto SafeDestroyEntity = [&](entt::entity e) {
					if (registry.any_of<DRAW::MeshCollection>(e)) {
						auto& meshes = registry.get<DRAW::MeshCollection>(e).meshes;
						for (auto mesh : meshes) {
							if (registry.valid(mesh)) registry.destroy(mesh);
						}
					}
					if (registry.valid(e)) registry.destroy(e);
					};

				auto RestartGame = [&]() {
					// destroy players, enemies, bullets, managers (same as your existing restart logic)
					std::vector<entt::entity> toReset;

					for (auto e : registry.view<GAME::Player>())		toReset.push_back(e);
					for (auto e : registry.view<GAME::Enemy>())			toReset.push_back(e);
					for (auto e : registry.view<GAME::Bullet>())		toReset.push_back(e);
					for (auto e : registry.view<GAME::EnemyBullet>())   toReset.push_back(e);
					for (auto e : registry.view<GAME::Bunker>())		toReset.push_back(e);
					for (auto e : registry.view<GAME::GameManager>())	toReset.push_back(e);

					for (auto e : toReset) SafeDestroyEntity(e);

					auto levelView = registry.view<GAME::CurrentLevel>();
					if (!levelView.empty()) {
						registry.get<GAME::CurrentLevel>(levelView.front()).number = 1;
					}

					registry.clear<GAME::GameOver>();

					// Recreate gameplay
					GameplayBehavior(registry);

					registry.ctx().get<GAME::Scoreboard>().score = 0;

					registry.ctx().get<GAME::GameState>().state = GAME::State::Playing;

					// ----------------------------------------------------
					// ------------------------------------------------------------------Reset high score entry UI so initials can be entered
					// -------------------------------------------------------------------again the next time the player reaches Game Over.
					// ----------------------------------------------------
					if (registry.ctx().contains<HighscoreEntryUI>())
					{
						auto& hsUI = registry.ctx().get<HighscoreEntryUI>();

						hsUI.submitted = false;               // allow submission again
						strcpy_s(hsUI.initials, sizeof(hsUI.initials), "");       // reset initials to default
					}

				};

				auto QuitToMainMenu = [&]() {
					std::vector<entt::entity> toDestroy;

					for (auto e : registry.view<GAME::Player>())		toDestroy.push_back(e);
					for (auto e : registry.view<GAME::Enemy>())			toDestroy.push_back(e);
					for (auto e : registry.view<GAME::Bullet>())		toDestroy.push_back(e);
					for (auto e : registry.view<GAME::EnemyBullet>())   toDestroy.push_back(e);
					for (auto e : registry.view<GAME::Bunker>())		toDestroy.push_back(e);
					for (auto e : registry.view<GAME::GameManager>())	toDestroy.push_back(e);
					for (auto e : registry.view<GAME::GameOver>())		toDestroy.push_back(e);

					for (auto e : toDestroy) SafeDestroyEntity(e);

					for (auto displayEntity : registry.view<DRAW::VulkanRenderer>()) {
						if (registry.any_of<DRAW::GPULevel>(displayEntity))
							registry.remove<DRAW::GPULevel>(displayEntity);
						if (registry.any_of<DRAW::CPULevel>(displayEntity))
							registry.remove<DRAW::CPULevel>(displayEntity);
						UnloadLevel(registry, displayEntity);
					}
					// Reset high score entry UI so initials can be entered again next time
					if (registry.ctx().contains<HighscoreEntryUI>())
					{
						auto& hsUI = registry.ctx().get<HighscoreEntryUI>();
						hsUI.submitted = false;
						strcpy_s(hsUI.initials, sizeof(hsUI.initials), "");
					}

					auto& gs = registry.ctx().get<GAME::GameState>();
					gs.state = GAME::State::MainMenu;
					};

				// Build menus based on current game state
				auto& gs = registry.ctx().get<GAME::GameState>();

				// Centered window helper (updated so windows receive focus and accept clicks)
				auto centerWindow = [&](const char* title, ImVec2 size) {
					ImGuiIO& io = ImGui::GetIO();
					ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
					ImGui::SetNextWindowSize(size);
					ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
					// keep no-decoration but allow the window to be focused and moved if needed
					ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings;
					ImGui::Begin(title, nullptr, flags);
					// ensure the menu gets focus so buttons receive input
					ImGui::SetWindowFocus(title);
					};


				// --------- splash screen draw ----------

				if (gs.state == GAME::State::Splash) {
					auto& splash = registry.ctx().get<GAME::SplashState>();

					float a = 1.0f;
					float t = splash.t;

					if (t < splash.fadeIn) {
						a = t / splash.fadeIn;
					}
					else if (t < splash.fadeIn + splash.hold) {
						a = 1.0f;
					}
					else {
						float outT = t - (splash.fadeIn + splash.hold);
						a = 1.0f - (outT / splash.fadeOut);
					}
					if (a < 0.0f) a = 0.0f;
					if (a > 1.0f) a = 1.0f;

					ImGuiIO& io = ImGui::GetIO();
					ImGui::SetNextWindowPos(ImVec2(0, 0));
					ImGui::SetNextWindowSize(io.DisplaySize);

					ImGuiWindowFlags flags =
						ImGuiWindowFlags_NoDecoration |
						ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoSavedSettings |
						ImGuiWindowFlags_NoInputs;

					ImGui::Begin("SplashScreen", nullptr, flags);

					// center text
					ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

					const char* line1 = splash.screens[splash.index];

					// draw with alpha
					ImVec4 col1 = ImVec4(1, 1, 1, a);
					ImVec4 col2 = ImVec4(1, 1, 1, a * 0.85f);

					auto& ui = registry.ctx().get<GAME::ImGuiState>();

					if (ui.splashFont) ImGui::PushFont(ui.splashFont);
					ImVec2 s1 = ImGui::CalcTextSize(line1);
					ImGui::SetCursorPos(ImVec2(center.x - s1.x * 0.5f, center.y - 30.0f));
					ImGui::TextColored(col1, "%s", line1);
					if (ui.splashFont) ImGui::PopFont();

					ImGui::End();
				}


				// MAIN MENU
				if (gs.state == GAME::State::MainMenu) 
				{
					auto& credits = registry.ctx().get<CreditsUIState>();
					if (credits.active) {
						goto AfterMainMenu;
					}


					centerWindow("MainMenu", ImVec2(860, 460)); // wider so we can show global scores on the right
					ImGui::Dummy(ImVec2(0, 8));

					// ---- Title (centered across the whole window) ----
					const char* title = "Arachnivasion";
					ImVec2 winSize = ImGui::GetWindowSize();
					ImVec2 textSize = ImGui::CalcTextSize(title);
					ImGui::SetCursorPosX((winSize.x - textSize.x) * 0.5f);
					ImGui::Text("%s", title);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					// ---- Two-column layout ----
					if (ImGui::BeginTable("MainMenuLayout", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV))
					{
						// Left fixed width for buttons; right stretches for leaderboard
						ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthFixed, 320.0f);
						ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);

						// =========================
						// LEFT COLUMN: buttons/audio/local highscores
						// =========================
						ImGui::TableNextColumn();

						if (ImGui::Button("Start", ImVec2(-1, 0))) {
							// create level and begin gameplay (same as Enter handler)
							auto vrView = registry.view<DRAW::VulkanRenderer>();
							if (!vrView.empty())
							{
								auto displayEntity = vrView.front();
								std::string levelPath = (*config).at("Level1").at("levelFile").as<std::string>();
								std::string modelPath = (*config).at("Level1").at("modelPath").as<std::string>();
								if (registry.any_of<DRAW::GPULevel>(displayEntity))
									registry.remove<DRAW::GPULevel>(displayEntity);
								if (registry.any_of<DRAW::CPULevel>(displayEntity))
									registry.remove<DRAW::CPULevel>(displayEntity);
								UnloadLevel(registry, displayEntity);

								registry.emplace<DRAW::CPULevel>(displayEntity, DRAW::CPULevel{ levelPath, modelPath });
								registry.emplace<DRAW::GPULevel>(displayEntity);

								registry.ctx().get<GAME::Scoreboard>().score = 0;
								GameplayBehavior(registry);
								gs.state = GAME::State::Playing;
							}
						}

						if (ImGui::Button("Credits", ImVec2(-1, 0))) {
							auto& credits = registry.ctx().get<CreditsUIState>();
							credits.active = true;
							credits.offsetY = -1.0f;
						}

						if (ImGui::Button("Exit", ImVec2(-1, 0)))
						{
							for (auto e : registry.view<APP::Window>()) {
								auto& gwWindow = registry.get<GW::SYSTEM::GWindow>(e);
								GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh{};
								gwWindow.GetWindowHandle(uwh);
								HWND hwnd = (HWND)uwh.window;
								PostMessage(hwnd, WM_CLOSE, 0, 0);
							}
						}

						// ---- Audio controls (unchanged) ----
						if (registry.ctx().contains<GAME::AudioBank>()) {
							auto& bank = registry.ctx().get<GAME::AudioBank>();

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Text("Audio");

							bool changed = false;
							float master = bank.master;
							float musicV = bank.musicVolume;
							float sfxV = bank.sfxVolume;

							if (ImGui::SliderFloat("Master Volume", &master, 0.0f, 1.0f, "%.2f")) changed = true;
							if (ImGui::SliderFloat("Music Volume", &musicV, 0.0f, 1.0f, "%.2f")) changed = true;
							if (ImGui::SliderFloat("SFX Volume", &sfxV, 0.0f, 1.0f, "%.2f")) changed = true;

							if (changed) {
								bank.master = master;
								bank.musicVolume = musicV;
								bank.sfxVolume = sfxV;
								bank.ApplyVolumes();
							}
						}

						// ---- Local High Scores (unchanged) ----
						GAME::EnsureHighscoresLoaded(registry, "saved_highscores.txt");

						if (registry.ctx().contains<GAME::Highscores>())
						{
							const auto& hs = registry.ctx().get<GAME::Highscores>();

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							const char* hsTitle = "High Scores (Local)";
							ImGui::Text("%s", hsTitle);
							ImGui::Spacing();

							int shown = (hs.count < GAME::Highscores::MAX) ? hs.count : GAME::Highscores::MAX;

							if (shown == 0) {
								ImGui::TextDisabled("No scores yet.");
							}
							else {
								ImGui::BeginChild("HighscoreList", ImVec2(0, 160), true);
								for (int i = 0; i < shown; ++i) {
									ImGui::Text("%2d.  %s  %d",
										i + 1,
										hs.entries[i].initials.c_str(),
										hs.entries[i].score);
								}
								ImGui::EndChild();
							}
						}

						// =========================
						// RIGHT COLUMN: Global High Scores
						// =========================
						ImGui::TableNextColumn();

						ImGui::Text("High Scores (Global)");
						ImGui::Separator();
						ImGui::Spacing();

						if (registry.ctx().contains<GAME::GlobalHighscores>())
						{
							const auto& gh = registry.ctx().get<GAME::GlobalHighscores>();
							int shown = (int)std::min<size_t>(gh.entries.size(), 10);

							if (shown == 0)
							{
								ImGui::TextDisabled("Global scores unavailable (or server asleep).");
							}
							else
							{
								ImGui::BeginChild("GlobalList", ImVec2(0, 0), true);
								for (int i = 0; i < shown; ++i)
								{
									ImGui::Text("%2d.  %s  %d",
										i + 1,
										gh.entries[i].initials.c_str(),
										gh.entries[i].score);
								}
								ImGui::EndChild();
							}
						}
						else
						{
							ImGui::TextDisabled("Global scores unavailable.");
						}

						ImGui::EndTable();
					}

					ImGui::End();

				AfterMainMenu:
					;
				}

				// PAUSE MENU
				if (gs.state == GAME::State::Paused) {
					// overlay effect: we render a small centered window
					centerWindow("PauseMenu", ImVec2(420, 300));
					ImGui::Text("Paused");
					ImGui::Spacing();
					if (ImGui::Button("Resume", ImVec2(-1, 0))) {
						gs.state = GAME::State::Playing;
					}
					if (ImGui::Button("Restart", ImVec2(-1, 0))) {
						RestartGame();
					}
					if (ImGui::Button("Quit to Main Menu", ImVec2(-1, 0))) {
						QuitToMainMenu();
					}
					// Audio controls (Pause menu)
					if (registry.ctx().contains<GAME::AudioBank>()) {
						auto& bank = registry.ctx().get<GAME::AudioBank>();

						ImGui::Separator();
						ImGui::Text("Audio");

						bool changed = false;
						float master = bank.master;
						float musicV = bank.musicVolume;
						float sfxV = bank.sfxVolume;

						if (ImGui::SliderFloat("Master Volume", &master, 0.0f, 1.0f, "%.2f")) changed = true;
						if (ImGui::SliderFloat("Music Volume", &musicV, 0.0f, 1.0f, "%.2f")) changed = true;
						if (ImGui::SliderFloat("SFX Volume", &sfxV, 0.0f, 1.0f, "%.2f")) changed = true;

						if (changed) {
							bank.master = master;
							bank.musicVolume = musicV;
							bank.sfxVolume = sfxV;
							bank.ApplyVolumes();
						}
					}
					ImGui::End();
				}

				// GAME OVER MENU
				if (gs.state == GAME::State::GameOver) {
					centerWindow("GameOver", ImVec2(420, 360));
					ImGui::Dummy(ImVec2(0, 8)); // small top padding
					const char* goTitle = "Game Over";
					ImVec2 goWinSize = ImGui::GetWindowSize();
					ImVec2 goTextSize = ImGui::CalcTextSize(goTitle);
					ImGui::SetCursorPosX((goWinSize.x - goTextSize.x) * 0.5f);
					ImGui::Text("%s", goTitle);
					ImGui::Spacing();
					// show final score
					int finalScore = 0;
					if (registry.ctx().contains<GAME::Scoreboard>())
						finalScore = registry.ctx().get<GAME::Scoreboard>().score;
					ImGui::Text("Final Score: %d", finalScore);
					// ------------------------------------------------------------------
// --------------------------------------------------------------------------------------High Score Initials Entry
// --------------------------------------------------------------------------------------Allows the player to enter 3-character initials for their score.
// --------------------------------------------------------------------------------------This is only shown once per Game Over to prevent duplicate entries.
// ------------------------------------------------------------------
					auto& hsUI = registry.ctx().get<HighscoreEntryUI>();

					// If the player has not yet submitted their initials
					if (!hsUI.submitted)
					{
						ImGui::Spacing();
						ImGui::Text("Enter Initials:");

						// InputText limited to 3 characters (+ null terminator)
						// - Uppercase letters only
						// - No spaces allowed
						ImGui::InputText("##Initials",
							hsUI.initials,
							4,
							ImGuiInputTextFlags_CharsUppercase |
							ImGuiInputTextFlags_CharsNoBlank);

						if (ImGui::Button("Submit Score"))
						{
							if (strlen(hsUI.initials) > 0)
							{
								// ----------------------------------------------------
								// Save LOCAL highscore (always succeeds)
								// ----------------------------------------------------
								GAME::SaveHighscoresOnGameOver(
									registry,
									hsUI.initials,
									finalScore,
									GetLocalHighscorePath()
								);

								if (GAME::SubmitGlobalHighscore(hsUI.initials, finalScore)) {
									GAME::RefreshGlobalHighscoresFromServer(registry);
									hsUI.submitted = true;
								}
								else {
									// optional: show an error message instead of "Score Submitted!"
								}
							}
						}
					}
					else
					{
						// Feedback once the score has been successfully recorded
						ImGui::Text("Score Submitted!");
					}

					ImGui::Spacing();

					// Highscore table (same style as Main Menu)
					if (registry.ctx().contains<GAME::Highscores>()) {
						const auto& hs = registry.ctx().get<GAME::Highscores>();

						ImGui::Separator();
						ImGui::Spacing();

						const char* hsTitle = "High Scores";
						ImVec2 winSize2 = ImGui::GetWindowSize();
						ImVec2 hsTextSize = ImGui::CalcTextSize(hsTitle);
						ImGui::SetCursorPosX((winSize2.x - hsTextSize.x) * 0.5f);
						ImGui::Text("%s", hsTitle);
						ImGui::Spacing();

						int shown = (hs.count < GAME::Highscores::MAX) ? hs.count : GAME::Highscores::MAX;

						if (shown == 0) {
							ImGui::TextDisabled("No scores yet.");
						}
						else {
							ImGui::BeginChild("GameOverHighscoreList", ImVec2(0, 140), true);

							for (int i = 0; i < shown; ++i) {
								// Display rank, initials, and score
								ImGui::Text("%2d.  %s  %d",
									i + 1,
									hs.entries[i].initials.c_str(),
									hs.entries[i].score);
							}

							ImGui::EndChild();
						}
					}
					else {
						ImGui::TextDisabled("Highscores unavailable.");
					}

					// ----------------------------------------------------
// Global High Scores (Read-Only, downloaded from web)
// ----------------------------------------------------
					if (registry.ctx().contains<GAME::GlobalHighscores>())
					{
						const auto& gh = registry.ctx().get<GAME::GlobalHighscores>();

						ImGui::Separator();
						ImGui::Spacing();
						ImGui::Text("Global High Scores");
						ImGui::Spacing();

						int shown = (int)std::min<size_t>(gh.entries.size(), 10);

						if (shown == 0)
						{
							ImGui::TextDisabled("Global scores unavailable.");
						}
						else
						{
							for (int i = 0; i < shown; ++i)
							{
								ImGui::Text("%2d.  %s  %d",
									i + 1,
									gh.entries[i].initials.c_str(),
									gh.entries[i].score);
							}
						}
					}

					ImGui::Spacing();
					if (ImGui::Button("Restart", ImVec2(-1, 0))) {
						RestartGame();
					}
					if (ImGui::Button("Quit to Main Menu", ImVec2(-1, 0))) {
						QuitToMainMenu();
					}
					ImGui::End();
				}
				// Scrolling Credits overlay
				if (registry.ctx().contains<CreditsUIState>()) {
					auto& credits = registry.ctx().get<CreditsUIState>();
					if (credits.active) {



						centerWindow("Credits", ImVec2(420, 360));

						// Back button 
						const float btnW = 80.0f;
						float btnPosX = ImGui::GetWindowContentRegionMax().x - btnW;
						ImGui::SetCursorPosX(btnPosX);

						if (ImGui::Button("Back", ImVec2(btnW, 0))) {
							credits.active = false;
							credits.offsetY = 0.0f;
							ImGui::End();
							goto AfterCredits;
						}

						ImGui::Spacing();

						const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
						credits.totalHeight = (float)credits.lines.size() * lineHeight;

						ImVec2 avail = ImGui::GetContentRegionAvail();
						ImGui::BeginChild("CreditsScroll", ImVec2(0, avail.y), true, ImGuiWindowFlags_NoScrollbar);

						float childHeight = ImGui::GetWindowHeight();

						if (credits.offsetY < 0.0f) {
							credits.offsetY = childHeight;
						}

						float dt = ImGui::GetIO().DeltaTime;

						credits.offsetY -= credits.speed * dt;

						if (credits.offsetY + credits.totalHeight < 0.0f) {
							credits.active = false;
							credits.offsetY = -1.0f;
						}
						else {
							ImGui::SetCursorPosY(credits.offsetY);

							float contentW = ImGui::GetContentRegionAvail().x;

							for (const auto& line : credits.lines) {
								float textW = ImGui::CalcTextSize(line.c_str()).x;
								float x = (contentW - textW) * 0.5f;
								if (x < 0) x = 0;
								ImGui::SetCursorPosX(x);
								ImGui::TextUnformatted(line.c_str());
							}
						}

						ImGui::EndChild();
						ImGui::End();

					AfterCredits:
						;
					}
				}
				ImGui::Render();

				imgui.drawThisFrame = true;
			}
		}

		for (auto entity : winView) {
			if (registry.any_of<APP::WindowClosed>(entity))
				++closedCount;
			else
				registry.patch<APP::Window>(entity); // calls on_update()
		}

	} while (winView.size() != closedCount); // exit when all windows are closed

}