#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/SettingsScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SettingsScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    Slider *sliderBGM, *sliderSFX;
    sliderBGM = new Slider(halfW * 0.8, halfH - 50 - 2, 300, 4);
    sliderBGM->SetOnValueChangedCallback(std::bind(
        &SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderBGM);
    AddNewObject(new Engine::Label("BGM: ", "romulus.ttf", 64, halfW * 0.7,
                                   halfH - 50, 255, 255, 255, 255, 0.5, 0.5));
    sliderSFX = new Slider(halfW * 0.8, halfH + 50 - 2, 300, 4);
    sliderSFX->SetOnValueChangedCallback(std::bind(
        &SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderSFX);
    AddNewObject(new Engine::Label("SFX: ", "romulus.ttf", 64, halfW * 0.7,
                                   halfH + 50, 255, 255, 255, 255, 0.5, 0.5));
    // Not safe if release resource while playing, however we only free while
    // change scene, so it's fine.
    bgmInstance =
        AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderSFX->SetValue(AudioHelper::SFXVolume);

    btn = new Engine::ImageButton("clickable/bgmmute_normal.png",
                                  "clickable/bgmmute_hover.png", halfW * 1.2, halfH - 100, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::MuteOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/sfxmute_normal.png",
                                  "clickable/sfxmute_hover.png", halfW * 1.2, halfH, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::MuteOnClick, this, 2));
    AddNewControlObject(btn);
}
void SettingsScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void SettingsScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void SettingsScene::BGMSlideOnValueChanged(float value)
{
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void SettingsScene::SFXSlideOnValueChanged(float value)
{
    AudioHelper::SFXVolume = value;
}

void SettingsScene::MuteOnClick(int sound_mode)
{
    if (sound_mode == 1)
    {
        BGMSlideOnValueChanged(0);
    }
    else if (sound_mode == 2)
    {
        SFXSlideOnValueChanged(0);
    }
}
