#ifndef SettingsScene_HPP
#define SettingsScene_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
#include <allegro5/allegro_audio.h>

class SettingsScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit SettingsScene() = default;
    void Update(float deltaTime) override;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
    void MuteOnClick(int sound_mode);

    Engine::Image* bg;
    Engine::Image* cloud1;
    Engine::Image* cloud2;

    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};

#endif   // SettingsScene_HPP
