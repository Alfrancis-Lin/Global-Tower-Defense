#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include <allegro5/allegro_audio.h>

class WinScene final : public Engine::IScene {
private:
    float ticks;
    ALLEGRO_SAMPLE_ID bgmId;
    std::string playerName;
    Engine::Label* nameLabel;
    float gameTime;
    int score;
    void OnKeyDown(int keyCode) override;
public:
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int btn);

    Engine::Image *bg;
    Engine::Image *cloud1;
    Engine::Image *cloud2;

    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};

#endif   // WINSCENE_HPP
