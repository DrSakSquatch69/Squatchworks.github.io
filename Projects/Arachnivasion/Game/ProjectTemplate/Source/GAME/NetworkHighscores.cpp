#include "../precompiled.h"    // ? CORRECT PATH

#include "NetworkHighscores.h"
#include "../CCL.h"

#include "GameComponents.h"
#include "HighScore.h"

#include <algorithm>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif
namespace GAME
{
    static const wchar_t* kServerHost = L"leaderboard-repo-arachnivasion.onrender.com";
    static const int kServerPort = 443;

    // ------------------------------------------------------------
    // Internal helper: HTTP GET
    // ------------------------------------------------------------
    static bool GetTextFromServer(const wchar_t* path, std::string& outText)
    {
#ifdef _WIN32
        outText.clear();

        HINTERNET hSession = WinHttpOpen(
            L"Arachnivasion/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(hSession, kServerHost, kServerPort, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            L"GET",
            path,
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE
        );

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        bool success = false;

        if (WinHttpSendRequest(
            hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0)
            && WinHttpReceiveResponse(hRequest, nullptr))
        {
            DWORD bytesAvailable = 0;
            while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0)
            {
                std::string buffer(bytesAvailable, '\0');
                DWORD bytesRead = 0;

                if (!WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead))
                    break;

                buffer.resize(bytesRead);
                outText += buffer;
            }

            success = !outText.empty();
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return success;
#else
        return false;
#endif
    }

    // ------------------------------------------------------------
    // Internal helper: HTTP POST
    // ------------------------------------------------------------
    static bool PostJSONToServer(const std::string& jsonBody)
    {
#ifdef _WIN32
        HINTERNET hSession = WinHttpOpen(
            L"Arachnivasion/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(hSession, kServerHost, kServerPort, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            L"POST",
            L"/submit",
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE
        );

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        const wchar_t* headers = L"Content-Type: application/json\r\n";

        bool success =
            WinHttpSendRequest(
                hRequest,
                headers,
                (DWORD)-1,
                (LPVOID)jsonBody.c_str(),
                (DWORD)jsonBody.size(),
                (DWORD)jsonBody.size(),
                0)
            && WinHttpReceiveResponse(hRequest, nullptr);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return success;
#else
        return false;
#endif
    }

    // ------------------------------------------------------------
    // Public API: Load global highscores
    // ------------------------------------------------------------
    void LoadGlobalHighscoresFromServer(entt::registry& registry)
    {
        auto& gh = registry.ctx().contains<GAME::GlobalHighscores>()
            ? registry.ctx().get<GAME::GlobalHighscores>()
            : registry.ctx().emplace<GAME::GlobalHighscores>();

        if (gh.loaded)
            return;

        std::string response;
        if (!GetTextFromServer(L"/highscores", response))
            return;

        auto json = nlohmann::json::parse(response, nullptr, false);
        if (!json.is_object() || !json.contains("highscores"))
            return;

        gh.entries.clear();

        for (const auto& e : json["highscores"])
        {
            GAME::HighscoreEntry entry;
            entry.initials = e.value("initials", "???");
            entry.score = e.value("score", 0);

            if (entry.initials.size() > 3)
                entry.initials = entry.initials.substr(0, 3);

            gh.entries.push_back(entry);
        }

        std::sort(gh.entries.begin(), gh.entries.end(),
            [](const GAME::HighscoreEntry& a, const GAME::HighscoreEntry& b)
            {
                return a.score > b.score;
            });

        gh.loaded = true;
    }

    // ------------------------------------------------------------
    // Public API: Submit global highscore
    // ------------------------------------------------------------
    bool SubmitGlobalHighscore(const char* initials, int score)
    {
        nlohmann::json payload;
        payload["initials"] = initials;
        payload["score"] = score;

        return PostJSONToServer(payload.dump());
    }

    void RefreshGlobalHighscoresFromServer(entt::registry& registry)
    {
        auto& gh = registry.ctx().contains<GAME::GlobalHighscores>()
            ? registry.ctx().get<GAME::GlobalHighscores>()
            : registry.ctx().emplace<GAME::GlobalHighscores>();

        gh.loaded = false;
        gh.entries.clear();

        LoadGlobalHighscoresFromServer(registry);
    }
}