#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
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

    x1 = x2 = 0;
    cloud1_speed = 40.0f;
    cloud2_speed = 60.0f;

    // background
    auto now = std::chrono::system_clock::now();
    std::time_t curr_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&curr_time);

    int hr = local_time->tm_hour;

    std::string time_period;
    int color = 0;
    if (hr >= 5 && hr <= 15)
    {
        time_period = "morning";
        color = 0;
    }
    else if (hr > 15 && hr <= 18)
    {
        time_period = "evening";
        color = 255;
    }
    else
    {
        time_period = "night";
        color = 255;
    }

    bg = new Engine::Image("background/" + time_period + "/1.png", 0, 0, 1600, 832, 0, 0);
    AddNewObject(bg);

    cloud1 = new Engine::Image("background/" + time_period + "/2.png", 0, 0, 1600, 832, 0, 0);
    AddNewObject(cloud1);
    
    cloud2 = new Engine::Image("background/" + time_period + "/3.png", 0, 0, 1600, 832, 0, 0);
    AddNewObject(cloud2);

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackOnClick, this, 1));
    AddNewControlObject(btn);

    Slider *sliderBGM, *sliderSFX;
    sliderBGM = new Slider(halfW * 0.8, halfH - 75 - 2, 300, 4);
    sliderBGM->SetOnValueChangedCallback(std::bind(
        &SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderBGM);
    AddNewObject(new Engine::Label("BGM: ", "romulus.ttf", 64, halfW * 0.7,
                                   halfH - 75, color, color, color, 255, 0.5, 0.5));
    sliderSFX = new Slider(halfW * 0.8, halfH + 50 - 2, 300, 4);
    sliderSFX->SetOnValueChangedCallback(std::bind(
        &SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sliderSFX);
    AddNewObject(new Engine::Label("SFX: ", "romulus.ttf", 64, halfW * 0.7,
                                   halfH + 50, color, color, color, 255, 0.5, 0.5));
    // Not safe if release resource while playing, however we only free while
    // change scene, so it's fine.
    bgmInstance =
        AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderSFX->SetValue(AudioHelper::SFXVolume);

    btn = new Engine::ImageButton("clickable/bgmmute_normal.png",
                                  "clickable/bgmmute_hover.png", halfW * 1.2, halfH - 125, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::MuteOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/sfxmute_normal.png",
                                  "clickable/sfxmute_hover.png", halfW * 1.2, halfH, 100, 100);
    btn->SetOnClickCallback(std::bind(&SettingsScene::MuteOnClick, this, 2));
    AddNewControlObject(btn);
}

void SettingsScene::Update(float deltaTime)
{
    Engine::IScene::Update(deltaTime);

    int w = Engine::GameEngine::GetInstance().GetScreenWidth();

    x1 -= cloud1_speed * deltaTime;
    x2 -= cloud2_speed * deltaTime;

    if (x1 <= -(1.5 * w)) x1 += 2.1 * w;
    if (x2 <= -(1.2 * w)) x2 += 2.4 * w;

    cloud1->Position.x = x1;
    cloud2->Position.x = x2;
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
