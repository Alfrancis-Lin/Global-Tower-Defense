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

    x1 = x2 = 0;
    cloud1_speed = 40.0f;
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
    else {
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

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&InfoScene::BackOnClick, this));
    AddNewControlObject(btn);


    AddNewObject(new Engine::Label("Developers:", "romulus.ttf", 72,
                                           halfW, (double)halfH / 2.5 -70, color, color,
                                           color, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Fufu", "romulus.ttf", 72,
                                           halfW, (double)halfH / 2.5, color, color,
                                           color, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Xinsheng Ooi", "romulus.ttf", 72,
                                           halfW, (double)halfH / 2.5 +70, color, color,
                                           color, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Alfrancis-Lin", "romulus.ttf", 72,
                                           halfW, (double)halfH / 2.5 +140, color, color,
                                           color, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Our Github:", "romulus.ttf", 72,
                                           halfW, (double)halfH / 2.5 +245, color, color,
                                           color, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("https://github.com/Alfrancis-Lin/Global-Tower-Defense", "romulus.ttf", 50,
                                           halfW, (double)halfH / 2.5 +315, color, color,
                                           color, 255, 0.5, 0.5));
}

void InfoScene::Update(float deltaTime)
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

void InfoScene::BackOnClick(void)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
