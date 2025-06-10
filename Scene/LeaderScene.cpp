//
// Created by 林威佑 on 2025/5/21.
//
#include <algorithm>
#include <allegro5/allegro_audio.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/LeaderScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void LeaderScene::Initialize()
{
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    LoadLeaderboard();
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
    AddNewObject(new Engine::Label("Back", "romulus.ttf", 48, halfW,
                                   (double)halfH * 3 / 2, 0, 0, 0, 255, 0.5,
                                   0.5));

    bgmInstance =
        AudioHelper::PlaySample("happy.ogg", true, AudioHelper::BGMVolume);

}

void LeaderScene::Terminate() { 
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void LeaderScene::BackOnClick(int stage)
{
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void LeaderScene::LoadLeaderboard()
{
    entries.clear();
    std::ifstream fin("Scene/leaderboard.txt");
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
                                       halfW * 0.35, yPos, 255, 255, 255, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(entry.name, "romulus.ttf", 64,
                                       halfW * 0.70, yPos, 255, 255, 255, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(entry.time, "romulus.ttf", 64,
                                       halfW * 1.20, yPos, 255, 255, 255, 255,
                                       0.5, 0.5));
        AddNewObject(new Engine::Label(std::to_string(entry.score),
                                       "romulus.ttf", 64, halfW * 1.65, yPos,
                                       255, 255, 255, 255, 0.5, 0.5));
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
