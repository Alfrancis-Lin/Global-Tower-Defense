#ifndef GLOBAL_ACCOUNT_MANAGER_H
#define GLOBAL_ACCOUNT_MANAGER_H

#include "AccountSystem.hpp"
#include <memory>

class GlobalAccountManager {
private:
    static GlobalAccountManager* instance;
    std::unique_ptr<AccountManager> accountManager;
    
    // Private constructor
    GlobalAccountManager() {
        // Default to OfflineAccount, but can be changed later
        accountManager = std::make_unique<OfflineAccount>();
    }
    
public:
    // Singleton access
    static GlobalAccountManager& GetInstance() {
        if (!instance) {
            instance = new GlobalAccountManager();
        }
        return *instance;
    }
    
    static void Initialize() {
        GetInstance(); // Ensure instance is created
    }
    
    static void Terminate() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }
    
    // Delegate all calls to the underlying AccountManager
    bool createAccount(const std::string& name, const std::string& pass) {
        return accountManager->createAccount(name, pass);
    }
    
    bool login(const std::string& name, const std::string& pass) {
        return accountManager->login(name, pass);
    }
    
    bool saveProgress() {
        return accountManager->saveProgress();
    }
    
    bool loadProgress() {
        return accountManager->loadProgress();
    }
    
    void logout() {
        accountManager->logout();
    }
    
    bool isLoggedIn() const {
        return accountManager->isLoggedIn();
    }
    
    const PlayerData& getPlayer() const {
        return accountManager->getPlayer();
    }
    
    void addExperience(int exp) {
        accountManager->addExperience(exp);
    }
    
    void setLevel(int newLevel) {
        accountManager->setLevel(newLevel);
    }
    
    void checkLevelUp() {
        accountManager->checkLevelUp();
    }
    
    // Allow switching account manager implementations
    void setAccountManager(std::unique_ptr<AccountManager> newManager) {
        if (accountManager && accountManager->isLoggedIn()) {
            accountManager->logout();
        }
        accountManager = std::move(newManager);
    }
    
    // Prevent copying
    GlobalAccountManager(const GlobalAccountManager&) = delete;
    GlobalAccountManager& operator=(const GlobalAccountManager&) = delete;
};

#endif // GLOBAL_ACCOUNT_MANAGER_H
