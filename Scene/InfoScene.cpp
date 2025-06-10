#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "InfoScene.hpp"
#include "UI/Component/ImageButton.hpp"

void InfoScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&InfoScene::BackOnClick, this));
    AddNewControlObject(btn);
    bgmInstance =
        AudioHelper::PlaySample("happy.ogg", true, AudioHelper::BGMVolume);
}
void InfoScene::Terminate()
{
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void InfoScene::BackOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
