#include "Engine/IScene.hpp"
#include "allegro5/allegro_audio.h"
#include <memory>

namespace Engine {
    class Image;
    class Label;
}

class IntroScene : public Engine::IScene {
private:
    Engine::Image* logo;
    Engine::Label* titleText;
    Engine::Label* companyText;
    Engine::Image* fadeOverlay;
    
    // Animation timing variables
    float animationTimer;
    float totalAnimationTime;
    
    // Animation states
    enum AnimationState {
        FADE_IN_COMPANY,
        SHOW_COMPANY,
        FADE_OUT_COMPANY,
        FADE_IN_LOGO,
        SHOW_LOGO,
        FADE_IN_TITLE,
        SHOW_ALL,
        FADE_OUT_ALL,
        FINISHED
    };
    
    AnimationState currentState;
    float stateTimer;
    
    // Animation parameters
    float fadeSpeed;
    bool skipRequested;
    
    // Audio
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> introSound;

public:
    explicit IntroScene() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Terminate() override;
    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int mx, int my) override;
    
private:
    void UpdateFadeInCompany(float deltaTime);
    void UpdateShowCompany(float deltaTime);
    void UpdateFadeOutCompany(float deltaTime);
    void UpdateFadeInLogo(float deltaTime);
    void UpdateShowLogo(float deltaTime);
    void UpdateFadeInTitle(float deltaTime);
    void UpdateShowAll(float deltaTime);
    void UpdateFadeOutAll(float deltaTime);
    void SkipToMainMenu();
};
