#include <functional>
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"



std::string GetCurrentTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    Engine::ImageButton *btn;


    AddNewObject(new Engine::Label("Enter Your Name ", "pirulen.ttf", 24, halfW, halfH + 10, 255, 255, 255, 255, 0.5, 0.5));

    score = dynamic_cast<PlayScene *>(
                       Engine::GameEngine::GetInstance().GetScene("play"))
                       ->GetMoney();
    nameLabel = new Engine::Label("_", "pirulen.ttf", 32, halfW, halfH + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(nameLabel);

    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");


}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}

void WinScene::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !playerName.empty()) {
        playerName.pop_back();
    } else if (playerName.length() < 10) {
        if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z)
            playerName += static_cast<char>('A' + keyCode - ALLEGRO_KEY_A);
    }
    nameLabel->Text = playerName + "_";
}

void WinScene::BackOnClick(int stage) {

    std::ofstream fout("/Users/linweiyou/CLionProjects/2025_I2P2_TowerDefense/Scene/leaderboard.txt", std::ios::app);
    std::cout << "Writing to leaderboard: "
          << playerName << " "
          << GetCurrentTimeString() << " "
          << score << std::endl;
    if (fout.is_open()) {
        fout << playerName << " " << score << " " << GetCurrentTimeString() << "\n";
        fout.close();


        if (fout.fail()) {
            std::cerr << "Error writing to leaderboard file" << std::endl;
        }
    } else {
        std::cerr << "Unable to open leaderboard file for writing" << std::endl;
    }


    Engine::GameEngine::GetInstance().ChangeScene("start");
}
