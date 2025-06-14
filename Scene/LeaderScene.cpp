#include <algorithm>
#include <allegro5/allegro_audio.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "Scene/LeaderScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void LeaderScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    LoadLeaderboard();

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

    AddNewObject(new Engine::Label("LEADERBOARD", "romulus.ttf", 60, halfW, 100,
                                   255, 255, 255, 255, 0.5, 0.5));
    DisplayCurrentPage();
    btn = new Engine::ImageButton("clickable/up_normal.png",
                                  "clickable/up_hover.png", halfW - 300,
                                  h - 200, 150, 150);
    btn->SetOnClickCallback(std::bind(&LeaderScene::PrevPage, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/down_normal.png",
                                  "clickable/down_hover.png", halfW + 150,
                                  h - 200, 150, 150);
    btn->SetOnClickCallback(std::bind(&LeaderScene::NextPage, this));
    AddNewControlObject(btn);

    btn = new Engine::ImageButton("clickable/back_normal.png",
                                  "clickable/back_hover.png", 50, 50, 100, 100);
    btn->SetOnClickCallback(std::bind(&LeaderScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
}

void LeaderScene::Update(float deltaTime)
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

void LeaderScene::Terminate() { 
    IScene::Terminate();
}

void LeaderScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void LeaderScene::LoadLeaderboard()
{
    entries.clear();
    std::ifstream fin("Scene/leader/leaderboard.txt");
    if (!fin.is_open()) {
        std::cerr << "Failed to open leaderboard.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        LeaderboardEntry entry;
        std::string datePart, timePart;

        if (!(iss >> entry.name >> entry.score >> datePart >> timePart)) {
            std::cerr << "Invalid format: " << line << std::endl;
            continue;
        }

        entry.time = datePart + " " + timePart;
        entries.push_back(entry);
    }

    std::sort(entries.begin(), entries.end(),
              [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
                  return a.score > b.score;
              });
}

void LeaderScene::DisplayCurrentPage()
{
    // background
    auto now = std::chrono::system_clock::now();
    std::time_t curr_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&curr_time);

    int hr = local_time->tm_hour;

    int color = 0;
    if (hr >= 5 && hr <= 15)
        color = 0;
    else color = 255;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    for (auto &obj : GetObjects()) {
        if (dynamic_cast<Engine::Label *>(obj) && obj != pageLabel) {
            RemoveObject(obj->GetObjectIterator());
        }
    }

    int startIdx = currentPage * entriesPerPage;
    int endIdx = std::min(startIdx + entriesPerPage, (int)entries.size());

    int yPos = 200;

    AddNewObject(new Engine::Label("Rank", "romulus.ttf", 96, halfW * 0.35, yPos,
                                   255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Name", "romulus.ttf", 96, halfW * 0.70, yPos,
                                   255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Time", "romulus.ttf", 96, halfW * 1.20, yPos,
                                   255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "romulus.ttf", 96, halfW * 1.65,
                                   yPos, 255, 255, 0, 255, 0.5, 0.5));

    yPos += 100;

    for (int i = startIdx; i < endIdx; i++) {
        const auto &entry = entries[i];
        AddNewObject(new Engine::Label(std::to_string(i + 1), "romulus.ttf", 64,
                                       halfW * 0.35, yPos, color, color, color, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(entry.name, "romulus.ttf", 64,
                                       halfW * 0.70, yPos, color, color, color, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(entry.time, "romulus.ttf", 64,
                                       halfW * 1.20, yPos, color, color, color, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(std::to_string(entry.score),
                                       "romulus.ttf", 64, halfW * 1.65, yPos,
                                       color, color, color, 255, 0.5, 0.5));
        yPos += 50;
    }
}

void LeaderScene::NextPage()
{
    int totalPages = (entries.size() + entriesPerPage - 1) / entriesPerPage;
    if (currentPage < totalPages - 1) {
        currentPage++;
        DisplayCurrentPage();
    }
}

void LeaderScene::PrevPage()
{
    if (currentPage > 0) {
        currentPage--;
        DisplayCurrentPage();
    }
}

void LeaderScene::OnKeyDown(int keyCode)
{

    if (keyCode == ALLEGRO_KEY_LEFT) {
        PrevPage();
    }
    else if (keyCode == ALLEGRO_KEY_RIGHT) {
        NextPage();
    }
}
