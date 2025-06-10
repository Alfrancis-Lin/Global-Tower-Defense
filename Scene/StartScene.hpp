#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP

#include <allegro5/allegro_audio.h>

#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
class StartScene final : public Engine::IScene {
  private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

  public:
    explicit StartScene() = default;
    void Update(float deltaTime) override;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);
    void LeaderOnClick(int stage);
    void QuitOnClick(void);
    void AccountOnClick(void);
    void InfoOnClick(void);

    Engine::Image *bg;
    Engine::Image *cloud1;
    Engine::Image *cloud2;

    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};
#endif
