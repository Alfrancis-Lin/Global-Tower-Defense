//
// Created by 林威佑 on 2025/5/21.
//

#ifndef LEADERSCENE_HPP
#define LEADERSCENE_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

struct LeaderboardEntry {
    std::string name;
    std::string time;
    int score;
};


class LeaderScene final : public Engine::IScene {

private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<LeaderboardEntry> entries;
    int currentPage = 0;
    int entriesPerPage = 10;
    Engine::Label* pageLabel;
    void LoadLeaderboard();
    void DisplayCurrentPage();

public:
    explicit LeaderScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
    void NextPage();
    void PrevPage();
};


#endif //LEADERSCENE_HPP
