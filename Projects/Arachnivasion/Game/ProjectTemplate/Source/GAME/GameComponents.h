#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_
#include "../APP/imgui.h"  

namespace GAME
{
	///*** Tags ***///
	struct Player {};
	struct Enemy {};
	struct Bullet {};
	struct CountedKill{};
	struct HasBullet {};
	struct EnemyBullet {};
	struct Bunker {};
	struct KilledByPlayer {};
	///*** Components ***///
	struct EnemyFireController
	{
		float timer = 0.0f;
	};

	struct GameManager {};

	struct GameOver {};

	struct Transform
	{
		GW::MATH::GMATRIXF matrix;
	};

	struct Firing
	{
		float cooldown;
	};
	
	struct EnemyFiring
	{
		float cooldown;
	};

	struct Velocity
	{
		GW::MATH::GVECTORF direction;
		float speed;
	};

	struct Collidable {};

	struct Obstacle {};

	struct ToDestroy {};

	struct Health
	{
		int value;
	};

	struct Invulnerability
	{
		float cooldown;
	};
	struct Lives
	{
		int value;
	};
	struct CurrentLevel
	{
		int number = 1;
	};

	struct LevelComplete {};

	enum class State { Splash,MainMenu, Playing, Paused, GameOver };

	struct GameState
	{
		State state = State::MainMenu;
	};

	struct SplashState {
		bool started = false;
		float t = 0.0f;

		float fadeIn = 1.0f;
		float hold = 1.0f;
		float fadeOut = 1.0f;

		int index = 0;

		std::vector<const char*> screens = {
			"Nightshade Games",
			"Powered by Gateware",
			"Rendered with Vulkan",
			"ECS by EnTT",
			"UI by Dear ImGui"
		};
	};

	// ---------- required mechanics ----------
	struct Swarm
	{
		float speed = 0.0f;
	};

	struct EggSac {};

	struct EggSacSpawner
	{
		float timer = 0.0f;
		bool active = true;
	};

	// ----------- sounds -----------
	struct AudioBank
	{
		// this is the SFX section
		GW::AUDIO::GAudio audio;
		GW::AUDIO::GSound shoot;
		GW::AUDIO::GSound enemyDie;
		GW::AUDIO::GSound eggSpawn;
		GW::AUDIO::GSound eggDie;

		// this section is for the actual music 
		GW::AUDIO::GMusic bgm;
		GW::AUDIO::GMusic menuMusic;
		bool bgmLoaded = false;
		bool menuLoaded = false;
		bool musicPaused = false;
		bool bgmLoop = true;
		bool menuLoop = true;

		// exposed mixer values (UI controls)
		float master = 1.0f;      // global master volume (0.0 - 1.0)
		float musicVolume = 1.0f; // music multiplier (0.0 - 1.0)
		float sfxVolume = 1.0f;   // sfx multiplier (0.0 - 1.0)

		bool enabled = true;
		enum class MusicNow { None, Menu, Gameplay } now = MusicNow::None;
		// Call this to apply the three floats to the audio subsystem.
		// Uses Gateware audio API calls available in Gateware.h.
		void ApplyVolumes()
		{
			// Update global/master volumes on the GAudio instance
			// These proxy calls return GW::GReturn; we ignore the return for now.
			audio.SetMasterVolume(master);
			audio.SetGlobalMusicVolume(master * musicVolume);
			audio.SetGlobalSoundVolume(master * sfxVolume);

			// Update per-music instance volumes (if loaded)
			if (bgmLoaded) {
				bgm.SetVolume(master * musicVolume);
			}
			if (menuLoaded) {
				menuMusic.SetVolume(master * musicVolume);
			}

			// Update SFX instances
			// It's okay to call SetVolume on sounds regardless of load state;
			// implementations typically ignore or return failure if not created.
			shoot.SetVolume(master * sfxVolume);
			enemyDie.SetVolume(master * sfxVolume);
			eggSpawn.SetVolume(master * sfxVolume);
			eggDie.SetVolume(master * sfxVolume);
		}
	};

	// IMGui stuff
	struct ImGuiState
	{
		bool initialized = false;
		bool drawThisFrame = false;
		bool fontsUploaded = false;
		ImFont* splashFont = nullptr;

		VkDevice device = VK_NULL_HANDLE;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

#ifdef _WIN32
		HWND hwnd = nullptr;
		WNDPROC oldWndProc = nullptr;
#endif
	};


	// ------------- Scoring -------------

	struct Scoreboard
	{
		int score = 0;
	};

	struct PointValue
	{
		int value = 0;
	};

	// Represents a single high score entry.
// Stores the player's initials alongside their score.
	struct HighscoreEntry
	{
		std::string initials; // 3-character player initials
		int score = 0;        // score achieved
	};

	// Stores the local high score table.
	// This persists between runs via a text file.
	struct Highscores
	{
		static constexpr int MAX = 10; // maximum number of stored scores

		// Sorted array of high score entries (highest score first)
		std::array<HighscoreEntry, MAX> entries;

		int count = 0;   // number of valid entries currently stored
		bool loaded = false; // prevents loading from disk multiple times
	};

	// Stores read-only global high scores downloaded from the web.
// This is optional data and does NOT affect gameplay.
	struct GlobalHighscores
	{
		std::vector<HighscoreEntry> entries; // downloaded scores
		bool loaded = false;                 // prevents re-downloading
	};


}

// namespace GAME
#endif // !GAME_COMPONENTS_H_
