#include <allegro5/allegro_audio.h>
#include <ctime>
// #include <curl/curl.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "StageSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void StageSelectScene::Initialize()
{
    // init page to 0
    page = 0;
    maxPage = 3;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    for (int i = 0; i < maxPage + 1; ++i)
        previews.emplace_back(new Engine::Sprite("stage_preview/preview" +
                                                     std::to_string(i) + ".png",
                                                 halfW, halfH - 50));
    difficulty[0] = 2;
    difficulty[1] = 4;
    difficulty[2] = -1;
    difficulty[3] = 5;

    info = new Engine::Label(
        "Level " + std::to_string(page + 1) + "\'s" + " Interesting Level: " +
            ((difficulty[page] == -1) ? "?" : std::to_string(difficulty[page])),
        "romulus.ttf", 56,
        Engine::GameEngine::GetInstance().GetScreenSize().x / 2, 100, 255, 255,
        255, 255, 0.5, 0.5);

    stage_preview = previews[page];

    Engine::ImageButton *btn;

    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", halfW * 0.1,
                                  halfH - 75, 150, 150);
    btn->SetOnClickCallback(
        std::bind(&StageSelectScene::PreviewStage, this, 1));

    AddNewControlObject(btn);
    btn = new Engine::ImageButton("clickable/next_normal.png",
                                  "clickable/next_hover.png", halfW * 1.7,
                                  halfH - 75, 150, 150);
    btn->SetOnClickCallback(
        std::bind(&StageSelectScene::PreviewStage, this, 2));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/tick_normal.png",
                                  "clickable/tick_hover.png", halfW - 100,
                                  h - 200, 150, 150);
    btn->SetOnClickCallback(
        std::bind(&StageSelectScene::PlayOnClick, this, page));
    AddNewControlObject(btn);
    // remember the play button so that can track which stage to play
    play_button = btn;

    if (Engine::GameEngine::GetInstance().annoyingMode) {
        annoying_label =
            new Engine::Label("Annoying: True", "romulus.ttf", 56, halfW * 1.5,
                              h - 200, 255, 255, 255, 255, 0.5, 0.5);
    }
    else {
        annoying_label =
            new Engine::Label("Annoying: False", "romulus.ttf", 56, halfW * 1.5,
                              h - 200, 255, 255, 255, 255, 0.5, 0.5);
    }
    AddNewObject(annoying_label);

    btn = new Engine::ImageButton("clickable/tick_normal.png",
                                  "clickable/tick_hover.png", halfW * 1.5,
                                  h - 150, 100, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::ToggleAnnoying, this));
    AddNewControlObject(btn);

    // Not safe if release resource while playing, however we only free while
    // change scene, so it's fine.
    bgmInstance =
        AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void StageSelectScene::Draw(void) const
{
    IScene::Draw();
    stage_preview->Draw();
    info->Draw();
}

void StageSelectScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void StageSelectScene::BackOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void StageSelectScene::PlayOnClick(int stage)
{
    PlayScene *scene = dynamic_cast<PlayScene *>(
        Engine::GameEngine::GetInstance().GetScene("play"));
    // the stage stored using page is 0-indexed
    scene->MapId = stage + 1;
    if (scene->MapId == 3) {
        PlayScene::multiendd = false;
        PlayScene::multiplay = true;
    }
    else {
        PlayScene::multiendd = true;
        PlayScene::multiplay = false;
    }
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void StageSelectScene::ScoreboardOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}

void StageSelectScene::PreviewStage(int stage)
{
    if (stage == 1) {
        if (page - 1 < 0)
            return;
        --page;
    }
    else if (stage == 2) {
        if (page + 1 > maxPage)
            return;
        ++page;
    }
    stage_preview = previews[page];

    // clean up info
    delete info;
    info = nullptr;
    info = new Engine::Label(
        "Level " + std::to_string(page + 1) + "\'s" +
            " Interesting Level: " + std::to_string(difficulty[page]),
        "romulus.ttf", 56,
        Engine::GameEngine::GetInstance().GetScreenSize().x / 2, 100, 255, 255,
        255, 255, 0.5, 0.5);

    play_button->SetOnClickCallback(
        std::bind(&StageSelectScene::PlayOnClick, this, page));
}

void StageSelectScene::ToggleAnnoying(void)
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::GameEngine::GetInstance().annoyingMode =
        !Engine::GameEngine::GetInstance().annoyingMode;
    bool annoying = Engine::GameEngine::GetInstance().annoyingMode;
    if (annoying_label)
        RemoveObject(annoying_label->GetObjectIterator());

    if (annoying) {
        annoying_label =
            new Engine::Label("Annoying: True", "romulus.ttf", 56, halfW * 1.5,
                              h - 200, 255, 255, 255, 255, 0.5, 0.5);
    }
    else {
        annoying_label =
            new Engine::Label("Annoying: False", "romulus.ttf", 56, halfW * 1.5,
                              h - 200, 255, 255, 255, 255, 0.5, 0.5);
    }
    AddNewObject(annoying_label);
}
