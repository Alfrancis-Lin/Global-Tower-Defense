#ifndef LEADERSCENE_HPP
#define LEADERSCENE_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
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
    int entriesPerPage = 5;
    Engine::Label* pageLabel;
    void LoadLeaderboard();
    void DisplayCurrentPage();

public:
    explicit LeaderScene() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Terminate() override;
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
    void NextPage();
    void PrevPage();
    
    Engine::Image* bg;
    Engine::Image* cloud1;
    Engine::Image* cloud2;

    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};


#endif //LEADERSCENE_HPP
