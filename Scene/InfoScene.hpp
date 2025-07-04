#ifndef INFOSCENE_HPP
#define INFOSCENE_HPP
#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
#include <allegro5/allegro_audio.h>
#include "UI/Component/Label.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include <memory>

class InfoScene final : public Engine::IScene {
public:
    explicit InfoScene() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void BackOnClick(void);


    Engine::Label* infoText;
    Engine::Image* bg;
    Engine::Image* cloud1;
    Engine::Image* cloud2;


    float x1, x2;
    float cloud1_speed;
    float cloud2_speed;
};

#endif   // INFOSCENE_HPP
