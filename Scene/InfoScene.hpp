#ifndef INFOSCENE_HPP
#define INFOSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>

class InfoScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit InfoScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(void);
};

#endif   // INFOSCENE_HPP
