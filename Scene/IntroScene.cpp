#include "IntroScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <memory>
#include <string>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"

void IntroScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    
    // Initialize animation variables
    animationTimer = 0.0f;
    totalAnimationTime = 8.0f; // Total intro duration
    currentState = FADE_IN_COMPANY;
    stateTimer = 0.0f;
    fadeSpeed = 255.0f; // Alpha change per second
    skipRequested = false;
    
    Engine::Image* background = new Engine::Image("background/black.png", 0, 0, w, h, 0, 0);
    AddNewObject(background);
    
    companyText = new Engine::Label("Developed by NTHU Students", "romulus.ttf", 64, 
                                   halfW, halfH * 0.3, 255, 255, 255, 0, 0.5, 0.5);
    AddNewObject(companyText);
    
    logo = new Engine::Image("icon.png", halfW - 150, halfH * 0.5, 300, 300, 0, 0);
    AddNewObject(logo);
    
    titleText = new Engine::Label("Global Tower Defense", "romulus.ttf", 128,
                                 halfW, halfH * 1.4, 255, 215, 0, 0, 0.5, 0.5);
    AddNewObject(titleText);
    
    Engine::Label* skipText = new Engine::Label("Press any key or click to skip", "romulus.ttf", 48,
                                               halfW, h - 100, 200, 200, 200, 255, 0.5, 0.5);
    AddNewObject(skipText);
}

void IntroScene::Update(float deltaTime)
{
    Engine::IScene::Update(deltaTime);
    
    if (skipRequested) {
        SkipToMainMenu();
        return;
    }
    
    animationTimer += deltaTime;
    stateTimer += deltaTime;
    
    switch (currentState) {
        case FADE_IN_COMPANY:
            UpdateFadeInCompany(deltaTime);
            break;
        case SHOW_COMPANY:
            UpdateShowCompany(deltaTime);
            break;
        case FADE_OUT_COMPANY:
            UpdateFadeOutCompany(deltaTime);
            break;
        case FADE_IN_LOGO:
            UpdateFadeInLogo(deltaTime);
            break;
        case SHOW_LOGO:
            UpdateShowLogo(deltaTime);
            break;
        case FADE_IN_TITLE:
            UpdateFadeInTitle(deltaTime);
            break;
        case SHOW_ALL:
            UpdateShowAll(deltaTime);
            break;
        case FADE_OUT_ALL:
            UpdateFadeOutAll(deltaTime);
            break;
        case FINISHED:
            SkipToMainMenu();
            break;
    }
}

void IntroScene::UpdateFadeInCompany(float deltaTime)
{
    // simulate anim using opcaity
    int alpha = (int)(stateTimer * fadeSpeed);
    if (alpha >= 255) {
        alpha = 255;
        currentState = SHOW_COMPANY;
        stateTimer = 0.0f;
    }
    companyText->Color = al_map_rgba(255, 255, 255, alpha);
}

void IntroScene::UpdateShowCompany(float deltaTime)
{
    // Show company text for 1.5 seconds
    if (stateTimer >= 1.5f) {
        currentState = FADE_OUT_COMPANY;
        stateTimer = 0.0f;
    }
}

void IntroScene::UpdateFadeOutCompany(float deltaTime)
{
    int alpha = 255 - (int)(stateTimer * fadeSpeed);
    if (alpha <= 0) {
        alpha = 0;
        currentState = FADE_IN_LOGO;
        stateTimer = 0.0f;
    }
    companyText->Color = al_map_rgba(255, 255, 255, alpha);
}

void IntroScene::UpdateFadeInLogo(float deltaTime)
{
    int alpha = (int)(stateTimer * fadeSpeed);
    float scale = 0.5f + (stateTimer * 0.5f);
    
    if (alpha >= 255) {
        alpha = 255;
        scale = 1.0f;
        currentState = SHOW_LOGO;
        stateTimer = 0.0f;
    }
    
    if (scale > 1.0f) scale = 1.0f;
    companyText->Color = al_map_rgba(255, 255, 255, alpha);
}

void IntroScene::UpdateShowLogo(float deltaTime)
{
    if (stateTimer >= 1.0f) {
        currentState = FADE_IN_TITLE;
        stateTimer = 0.0f;
    }
}

void IntroScene::UpdateFadeInTitle(float deltaTime)
{
    int alpha = (int)(stateTimer * fadeSpeed);
    if (alpha >= 255) {
        alpha = 255;
        currentState = SHOW_ALL;
        stateTimer = 0.0f;
    }
    titleText->Color = al_map_rgba(255, 215, 0, alpha);
}

void IntroScene::UpdateShowAll(float deltaTime)
{
    if (stateTimer >= 2.0f) {
        currentState = FADE_OUT_ALL;
        stateTimer = 0.0f;
    }
    
    float pulse = 0.8f + 0.2f * sin(animationTimer * 3.0f);
    int glowAlpha = (int)(255 * pulse);
    titleText->Color = al_map_rgba(255, 215, 0, glowAlpha);
}

void IntroScene::UpdateFadeOutAll(float deltaTime)
{
    int alpha = 255 - (int)(stateTimer * fadeSpeed);
    if (alpha <= 0) {
        alpha = 0;
        currentState = FINISHED;
    }
    
    titleText->Color = al_map_rgba(255, 215, 0, alpha);
}

void IntroScene::OnKeyDown(int keyCode)
{
    skipRequested = true;
}

void IntroScene::OnMouseDown(int button, int mx, int my)
{
    skipRequested = true;
}

void IntroScene::SkipToMainMenu()
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void IntroScene::Terminate()
{
    if (introSound) {
        AudioHelper::StopSample(introSound);
        introSound = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    Engine::IScene::Terminate();
}
