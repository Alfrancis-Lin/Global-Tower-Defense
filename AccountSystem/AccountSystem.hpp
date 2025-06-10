#ifndef ACCOUNT_SYSTEM_H
#define ACCOUNT_SYSTEM_H

#include "Engine/LOG.hpp"
#include "allegro5/fshook.h"
#include "lib/json.hpp"
#include <allegro5/allegro.h>
//#include <curl/curl.h>
#include <fstream>
#include <string>

using json = nlohmann::json;

struct PlayerData {
    std::string name;
    std::string passwordHash;
    int level;
    int experience;
    bool isOnline;
    std::string lastLogin;

    PlayerData() : level(1), experience(0), isOnline(false) {}
};

class AccountManager {
  protected:
    PlayerData player;
    bool loggedIn;
    std::string save_dir;

  public:
    AccountManager() : loggedIn(false), save_dir("saves/")
    {
        al_make_directory(save_dir.c_str());
    }

    virtual ~AccountManager() = default;

    virtual bool createAccount(const std::string &name, const std::string &pass) = 0;
    virtual bool login(const std::string &name, const std::string &pass) = 0;
    virtual bool saveProgress() = 0;
    virtual bool loadProgress() = 0;
    virtual void logout() = 0;

    bool isLoggedIn(void) const { return loggedIn; }
    const PlayerData &getPlayer(void) const { return player; }

    void checkLevelUp()
    {
        int require_xp = player.level * 100;
        if (player.experience >= require_xp) {
            player.level++;
            player.experience -= require_xp;
        }
    }

    void addExperience(int exp)
    {
        player.experience += exp;
        checkLevelUp();
    }

    void setLevel(int newLevel)
    {
        if (newLevel > 0) {
            player.level = newLevel;
        }
    }

    std::string hashPassword(const std::string &password)
    {
        std::hash<std::string> hasher;
        return std::to_string(hasher(password));
    }

    json playerToJson(void) const
    {
        json j;
        j["name"] = player.name;
        j["name"] = player.name;
        j["passwordHash"] = player.passwordHash;
        j["level"] = player.level;
        j["experience"] = player.experience;
        j["isOnline"] = player.isOnline;
        j["lastLogin"] = player.lastLogin;
        return j;
    }

    void jsonToPlayer(const json &j)
    {
        player.name = j.value("name", "");
        player.passwordHash = j.value("passwordHash", "");
        player.level = j.value("level", 1);
        player.experience = j.value("experience", 0);
        player.isOnline = j.value("isOnline", false);
        player.lastLogin = j.value("lastLogin", "");
    }

    std::string getCurrentTime()
    {
        time_t now = time(0);
        return std::to_string(now);
    }
};

class OfflineAccount : public AccountManager {
  private:
    std::string getFilePath(const std::string &name)
    {
        return save_dir + name + ".json";
    }

  public:
    ~OfflineAccount() = default;
    bool createAccount(const std::string &name, const std::string &pass) override
    {
        if (name.empty() || pass.empty())
            return false;
        if (name.length() < 3 || name.length() >= 10)
            return false;
        if (pass.length() < 6)
            return false;

        std::ifstream f(getFilePath(name));
        if (f.good()) {
            f.close();
            Engine::LOG(Engine::INFO) << "Account already exists";
            return false;
        }

        player = PlayerData();
        player.name = name;
        player.passwordHash = hashPassword(pass);
        player.lastLogin = getCurrentTime();
        player.isOnline = false;

        loggedIn = true;

        if (saveProgress())
            return true;
        else {
            loggedIn = false;
            return false;
        }
    }

    bool login(const std::string &name, const std::string &pass) override
    {
        std::ifstream f(getFilePath(name));
        if (!f.good())
            return false;

        json root;

        try {
            f >> root;
            f.close();
        }
        catch (const json::exception &e) {
            f.close();
            Engine::LOG(Engine::WARN) << e.what();
            return false;
        }

        std::string storedHash = root.value("passwordHash", "");
        if (storedHash != hashPassword(pass))
            return false;

        jsonToPlayer(root);
        player.lastLogin = getCurrentTime();
        player.isOnline = false;
        loggedIn = true;

        bool saved = saveProgress();
        return true;
    }

    bool saveProgress(void) override
    {
        if (!loggedIn)
            return false;

        std::ofstream f(getFilePath(player.name));
        if (!f.good())
            return false;

        json j = playerToJson();
        f << j.dump(4);
        f.close();
        return true;
    }

    bool loadProgress(void) override
    {
        if (!loggedIn)
            return false;

        std::ifstream f(getFilePath(player.name));
        if (!f.good())
            return false;

        json j;
        try {
            f >> j;
            f.close();
            jsonToPlayer(j);
            return true;
        }
        catch (const json::exception &e) {
            f.close();
            return false;
        }
    }

    void logout(void) override
    {
        if (loggedIn) {
            saveProgress();
            loggedIn = false;
            player = PlayerData();
        }
    }
};

#endif // ACCOUNT_SYSTEM_H
