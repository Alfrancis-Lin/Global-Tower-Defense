//
// Created by 林威佑 on 2025/5/21.
//
#include <allegro5/allegro_audio.h>
#include <functional>
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>


#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/LeaderScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void LeaderScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    LoadLeaderboard();
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Label("LEADERBOARD", "pirulen.ttf", 60, halfW, 100, 255, 255, 255, 255, 0.5, 0.5));
    DisplayCurrentPage();
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 300, h - 150, 150, 50);
    btn->SetOnClickCallback(std::bind(&LeaderScene::PrevPage, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("<-", "pirulen.ttf", 24, halfW - 225, h - 125, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 150, h - 150, 150, 50);
    btn->SetOnClickCallback(std::bind(&LeaderScene::NextPage, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("->", "pirulen.ttf", 24, halfW + 225, h - 125, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&LeaderScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

}
void LeaderScene::Terminate() {
    IScene::Terminate();
}
void LeaderScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void LeaderScene::LoadLeaderboard() {
    entries.clear();
    std::ifstream fin("/Users/linweiyou/CLionProjects/2025_I2P2_TowerDefense/Scene/leaderboard.txt");
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
            std::cerr << "Invalidformat: " << line << std::endl;
            continue;
        }

        entry.time = datePart + " " + timePart;
        entries.push_back(entry);
    }



    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.score > b.score;
    });
}


void LeaderScene::DisplayCurrentPage() {

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    for (auto& obj : GetObjects()) {
        if (dynamic_cast<Engine::Label*>(obj) && obj != pageLabel) {
            RemoveObject(obj->GetObjectIterator());
        }
    }

    AddNewObject(new Engine::Label("<-", "pirulen.ttf", 24, halfW - 225, h - 125, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("->", "pirulen.ttf", 24, halfW + 225, h - 125, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    int startIdx = currentPage * entriesPerPage;
    int endIdx = std::min(startIdx + entriesPerPage, (int)entries.size());



    int yPos = 200;


    AddNewObject(new Engine::Label("Rank", "pirulen.ttf", 26, halfW - 300, yPos, 255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", 26, halfW - 150, yPos, 255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Time", "pirulen.ttf", 26, halfW + 100, yPos, 255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 26, halfW + 380, yPos, 255, 255, 0, 255, 0.5, 0.5));

    yPos += 40;


    for (int i = startIdx; i < endIdx; i++) {
        const auto& entry = entries[i];
        AddNewObject(new Engine::Label(std::to_string(i + 1), "pirulen.ttf", 22, halfW - 300, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(entry.name, "pirulen.ttf", 22, halfW - 150, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(entry.time, "pirulen.ttf", 22, halfW + 140, yPos, 255, 255, 255, 255, 0.5, 0.5));
        AddNewObject(new Engine::Label(std::to_string(entry.score), "pirulen.ttf", 22, halfW + 400, yPos, 255, 255, 255, 255, 0.5, 0.5));
        yPos += 30;
    }
}



void LeaderScene::NextPage() {
    int totalPages = (entries.size() + entriesPerPage - 1) / entriesPerPage;
    if (currentPage < totalPages - 1) {
        currentPage++;
        DisplayCurrentPage();

    }
}

void LeaderScene::PrevPage() {
    if (currentPage > 0) {
        currentPage--;
        DisplayCurrentPage();

    }
}



void LeaderScene::OnKeyDown(int keyCode) {

    if (keyCode == ALLEGRO_KEY_LEFT) {
        PrevPage();
    } else if (keyCode == ALLEGRO_KEY_RIGHT) {
        NextPage();
    }
}