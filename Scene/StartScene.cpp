#include "StartScene.hpp"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void StartScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Label("Global Tower Defense", "romulus.ttf", 160, halfW,
                                   (double)halfH / 2.5, 255, 255, 255, 255, 0.5,
                                   0.5));

    btn = new Engine::ImageButton("clickable/play_normal.png",
                                  "clickable/play_hover.png", halfW - 100,
                                  (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/setting_normal.png",
                                  "clickable/setting_hover.png", (double)halfW / 2,
                                  (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/leader_normal.png",
                                  "clickable/leader_hover.png", halfW * 1.25,
                                  (double)halfH * 1.2, 200, 200);
    btn->SetOnClickCallback(std::bind(&StartScene::LeaderOnClick, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/quit_normal.png",
                                  "clickable/quit_hover.png", 50,
                                  h - 150, 100, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::QuitOnClick, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/account_normal.png",
                                  "clickable/account_hover.png", w - 150,
                                  h - 250, 100, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::AccountOnClick, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/info_normal.png",
                                  "clickable/info_hover.png", w - 150,
                                  h - 125, 100, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::InfoOnClick, this));
    AddNewControlObject(btn);
}

void StartScene::Terminate() { IScene::Terminate(); }

void StartScene::PlayOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void StartScene::SettingsOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}

void StartScene::LeaderOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("leader");
}

void StartScene::QuitOnClick(void)
{
    exit(0);
}

void StartScene::AccountOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("account");
}

void StartScene::InfoOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("info");
}
