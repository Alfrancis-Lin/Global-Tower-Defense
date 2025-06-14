#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

std::string GetCurrentTimeString()
{
    std::time_t now = std::time(nullptr);
    std::tm *ltm = std::localtime(&now);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

void WinScene::Initialize()
{
    ticks = 0;
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

    AddNewObject(new Engine::Label("You Win!", "romulus.ttf", 96, halfW,
                                   (double)halfH / 4, 255, 255, 255, 255, 0.5,
                                   0.5));
    Engine::ImageButton *btn;
    AddNewObject(new Engine::Label("Enter Your Name ", "romulus.ttf", 64, halfW,
                                   (double)halfH / 1.5, 255, 255, 255, 255, 0.5,
                                   0.5));

    score = dynamic_cast<PlayScene *>(
                Engine::GameEngine::GetInstance().GetScene("play"))
                ->GetMoney();
    nameLabel = new Engine::Label("_", "romulus.ttf", 86, halfW, halfH + 50,
                                  255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(nameLabel);

    btn = new Engine::ImageButton("clickable/quit_normal.png",
                                  "clickable/quit_hover.png", halfW - 175,
                                  (double)h - 200, 150, 150);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 0));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/tick_normal.png",
                                  "clickable/tick_hover.png", halfW + 25,
                                  (double)h - 200, 150, 150);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate()
{
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime)
{
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(
            Engine::GameEngine::GetInstance().GetScene("play"))
                ->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::OnKeyDown(int keyCode)
{
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !playerName.empty()) {
        playerName.pop_back();
    }
    else if (playerName.length() < 10) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
            char c = static_cast<char>('A' + keyCode - ALLEGRO_KEY_A);
            if (isalpha(c))
                playerName += c;
        }
    }
    if (nameLabel)
        nameLabel->Text = playerName + "_";
}

void WinScene::BackOnClick(int btn)
{
    if (btn == 1) {
        std::ofstream fout("Scene/leaderboard.txt", std::ios::app);
        std::cout << "Writing to leaderboard: " << playerName << " "
                  << GetCurrentTimeString() << " " << score << std::endl;
        if (fout.is_open()) {
            fout << playerName << " " << score << " " << GetCurrentTimeString()
                 << "\n";
            fout.close();

            if (fout.fail()) {
                std::cerr << "Error writing to leaderboard file" << std::endl;
            }
        }
        else {
            std::cerr << "Unable to open leaderboard file for writing"
                      << std::endl;
        }
    }

    Engine::GameEngine::GetInstance().ChangeScene("start");
}
