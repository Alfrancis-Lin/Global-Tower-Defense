#include "StartScene.hpp"
#include <allegro5/allegro_audio.h>
#include <chrono>
#include <ctime>
#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void StartScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    x1 = x2 = 0;
    cloud1_speed = 20.0f;
    cloud2_speed = 60.0f;

    // background
    auto now = std::chrono::system_clock::now();
    std::time_t curr_time = std::chrono::system_clock::to_time_t(now);
    std::tm *local_time = std::localtime(&curr_time);

    int hr = local_time->tm_hour;

    std::string time_period;
    int color = 0;

    if (hr >= 5 && hr <= 15) {
        time_period = "morning";
        color = 0;
    }
    else if (hr > 15 && hr <= 18) {
        time_period = "evening";
        color = 255;
    }
    else if (hr > 18 && hr < 5){
        time_period = "night";
        color = 255;
    }

    bg = new Engine::Image("background/" + time_period + "/1.png", 0, 0, 1600,
                           832, 0, 0);
    AddNewObject(bg);

    cloud1 = new Engine::Image("background/" + time_period + "/2.png", 0, 0,
                               1600, 832, 0, 0);
    AddNewObject(cloud1);

    cloud2 = new Engine::Image("background/" + time_period + "/3.png", 0, 0,
                               1600, 832, 0, 0);
    AddNewObject(cloud2);

    AddNewObject(new Engine::Label("Global Tower Defense", "romulus.ttf", 160,
                                   halfW, (double)halfH / 1.8, color, color,
                                   color, 255, 0.5, 0.5));

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("clickable/play_normal.png",
                                  "clickable/play_hover.png", halfW - 100,
                                  (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton(
        "clickable/setting_normal.png", "clickable/setting_hover.png",
        (double)halfW / 2, (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/leader_normal.png",
                                  "clickable/leader_hover.png", halfW * 1.25,
                                  (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::LeaderOnClick, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/quit_normal.png",
                                  "clickable/quit_hover.png", 50, h - 150, 100,
                                  100);
    btn->SetOnClickCallback(std::bind(&StartScene::QuitOnClick, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/account_normal.png",
                                  "clickable/account_hover.png", w - 150,
                                  h - 250, 100, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::AccountOnClick, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/info_normal.png",
                                  "clickable/info_hover.png", w - 150, h - 125,
                                  100, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::InfoOnClick, this));
    AddNewControlObject(btn);

    if (!bgmInstance)
        bgmInstance =
            AudioHelper::PlaySample("happy.ogg", true, AudioHelper::BGMVolume);
}

void StartScene::Update(float deltaTime)
{
    Engine::IScene::Update(deltaTime);

    int w = Engine::GameEngine::GetInstance().GetScreenWidth();

    x1 -= cloud1_speed * deltaTime;
    x2 -= cloud2_speed * deltaTime;

    if (x1 <= -(1.5 * w))
        x1 += 2.1 * w;
    if (x2 <= -(1.2 * w))
        x2 += 2.4 * w;

    cloud1->Position.x = x1;
    cloud2->Position.x = x2;
}

void StartScene::Terminate()
{
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void StartScene::PlayOnClick(int stage)
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void StartScene::SettingsOnClick(int stage)
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}

void StartScene::LeaderOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("leader");
}

void StartScene::QuitOnClick(void) { exit(0); }

void StartScene::AccountOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("account");
}

void StartScene::InfoOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("info");
}
