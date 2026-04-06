#pragma once
#include <array>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "GameComponents.h"

namespace GAME
{
    inline std::filesystem::path ExeDir()
    {
#ifdef _WIN32
        char buf[MAX_PATH]{};
        DWORD len = GetModuleFileNameA(nullptr, buf, MAX_PATH);
        std::filesystem::path p(buf);
        return (len > 0) ? p.parent_path() : std::filesystem::current_path();
#else
        return std::filesystem::current_path();
#endif
    }

    inline std::filesystem::path ResolveHighscorePath(const std::string& path)
    {
        std::filesystem::path p(path);
        if (p.is_absolute()) return p;
        return ExeDir() / p;
    }

    // Inserts a new high score entry into the leaderboard.
// Maintains descending score order and caps at MAX entries.
    inline void InsertScore(Highscores& hs, const std::string& initials, int score)
    {
        if (score <= 0)
            return;

        HighscoreEntry newEntry{ initials, score };

        // Add new entry if we have room
        if (hs.count < Highscores::MAX)
        {
            hs.entries[hs.count++] = newEntry;
        }
        else
        {
            // Replace lowest score only if the new score is higher
            if (score <= hs.entries[hs.count - 1].score)
                return;

            hs.entries[hs.count - 1] = newEntry;
        }

        // Sort all valid entries from highest to lowest score
        std::sort(hs.entries.begin(), hs.entries.begin() + hs.count,
            [](const HighscoreEntry& a, const HighscoreEntry& b)
            {
                return a.score > b.score;
            });
    }

    // Loads high scores from disk.
// Expected format per line: <INITIALS> <SCORE>
    inline bool LoadHighscoresFromFile(Highscores& hs, const std::string& path)
    {
        const auto abs = ResolveHighscorePath(path);
        std::ifstream in(abs);

        if (!in.is_open())
            return false;

        hs.count = 0;

        std::string initials;
        int score = 0;

        // Read each line as "ABC 12345"
        while (in >> initials >> score)
        {
            if (hs.count < Highscores::MAX)
                // Clamp initials to 3 characters to maintain UI consistency
                if (initials.length() > 3)
                    initials = initials.substr(0, 3);

            hs.entries[hs.count++] = { initials, score };
        }

        // Ensure correct order after loading
        std::sort(hs.entries.begin(), hs.entries.begin() + hs.count,
            [](const HighscoreEntry& a, const HighscoreEntry& b)
            {
                return a.score > b.score;
            });

        return true;
    }

    // Saves the current high score table to disk.
// Each entry is written as: <INITIALS> <SCORE>
    inline bool SaveHighscoresToFile(const Highscores& hs, const std::string& path)
    {
        const auto abs = ResolveHighscorePath(path);

        // Ensure parent directory exists
        std::filesystem::create_directories(abs.parent_path());

        std::ofstream out(abs, std::ios::trunc);
        if (!out.is_open())
            return false;

        for (int i = 0; i < hs.count; ++i)
        {
            out << hs.entries[i].initials << " "
                << hs.entries[i].score << "\n";
        }

        return true;
    }

    inline void EnsureHighscoresLoaded(entt::registry& registry, const std::string& path)
    {
        Highscores& hs = registry.ctx().contains<Highscores>()
            ? registry.ctx().get<Highscores>()
            : registry.ctx().emplace<Highscores>();

        if (!hs.loaded) {
            LoadHighscoresFromFile(hs, path);
            hs.loaded = true;
        }
    }

    // Saves a player's score and initials at the end of a game.
// This function ensures scores are loaded before inserting.
    inline void SaveHighscoresOnGameOver(
        entt::registry& registry,
        const std::string& initials,
        int finalScore,
        const std::string& path)
    {
        EnsureHighscoresLoaded(registry, path);

        auto& hs = registry.ctx().get<Highscores>();

        // Insert the new score into the leaderboard
        InsertScore(hs, initials, finalScore);

        // Persist updated scores to disk
        SaveHighscoresToFile(hs, path);
    }
}
