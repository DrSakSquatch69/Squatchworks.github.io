#pragma once

namespace GAME
{
    // Loads global highscores from the REST server (GET /highscores)
    void LoadGlobalHighscoresFromServer(entt::registry& registry);

    // Submits a global highscore to the REST server (POST /submit)
    bool SubmitGlobalHighscore(const char* initials, int score);

    // forces a re-download of global highscores now
    void RefreshGlobalHighscoresFromServer(entt::registry& registry);
}