#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <curl/curl.h>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "StageSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// api setup
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetchTrivia() {
    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://numbersapi.com/random/math");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

void StageSelectScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    btn = new Engine::ImageButton("clickable/dirt.png",
                                  "clickable/floor.png", halfW - 200,
                                  halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 1", "romulus.ttf", 48, halfW,
                                   halfH / 2, 0, 0, 0, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("clickable/dirt.png",
                                  "clickable/floor.png", halfW - 200,
                                  halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 2", "romulus.ttf", 48, halfW,
                                   halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("clickable/dirt.png",
                                  "clickable/floor.png", halfW - 200,
                                  (double)halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage ?", "romulus.ttf", 48, halfW,
                                   (double)halfH / 2 + 250, 0, 0, 0, 255, 0.5,
                                   0.5));

    btn = new Engine::ImageButton("clickable/dirt.png",
                                  "clickable/floor.png", halfW - 200,
                                  (double)halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "romulus.ttf", 48, halfW,
                                   (double)halfH * 3 / 2, 0, 0, 0, 255, 0.5,
                                   0.5));

    // fetch random trivia api and truncate if text is too long
    std::string fact = fetchTrivia();
    if (fact.size() > 60) fact = fact.substr(0, 60) + "...";
    AddNewObject(new Engine::Label("Random Fact: " + fact, "romulus.ttf", 24, halfW,
                                   (double)halfH * 1.8, 255, 255, 255, 255, 0.5,
                                   0.5));

    // Not safe if release resource while playing, however we only free while
    // change scene, so it's fine.
    bgmInstance =
        AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void StageSelectScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void StageSelectScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void StageSelectScene::PlayOnClick(int stage)
{
    PlayScene *scene = dynamic_cast<PlayScene *>(
        Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void StageSelectScene::ScoreboardOnClick()
{
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
