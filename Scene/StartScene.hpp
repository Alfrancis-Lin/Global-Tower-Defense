#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class StartScene final : public Engine::IScene {
public:
    explicit StartScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);
    void LeaderOnClick(int stage);
    void QuitOnClick(void);
    void AccountOnClick(void);
    void InfoOnClick(void);
};
#endif

