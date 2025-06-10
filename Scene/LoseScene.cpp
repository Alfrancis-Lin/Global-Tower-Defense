#include <functional>
#include <string>
#include <random>
#include <algorithm>
#include <allegro5/allegro_primitives.h>  // 添加基本繪圖功能
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "LoseScene.hpp"
#include "PlayScene.hpp"
#include "StartScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

// 恐龍遊戲相關常數
const int GROUND_HEIGHT = 400;
const int DINO_WIDTH = 80;
const int DINO_HEIGHT = 86;
const int CACTUS_WIDTH = 25;
const int CACTUS_HEIGHT = 35;
const int BIRD_WIDTH = 50;
const int BIRD_HEIGHT = 30;
const int GAME_WIDTH = 800;
const int GAME_HEIGHT = 450;
int currentDinoHeight = 450;
int ss=0;

void LoseScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    // 初始化恐龍遊戲狀態
    dinoX = 100;
    dinoY = GROUND_HEIGHT - DINO_HEIGHT;
    dinoVelocity = 0;
    isJumping = false;
    isDucking = false;
    gameSpeed = 5;
    score = 0;
    gameOver = false;
    obstacles.clear();

    // 修正：將初始時間設置得更小，讓障礙物更快生成
    nextObstacleTime = 1.8f;
    state = GameState::PLAYING;
    gameOverWaitTime = 0;
    gameOverWaitDuration = 4.0f;
    gameStartWaitTime = 0;
    gameStartWaitDuration = 4.0f;
    // 背景
    AddNewObject(new Engine::Image("play/sand.png", 0, 0, w, h));

    // 地面
    groundY = GROUND_HEIGHT;
    groundX1 = 0;
    groundX2 = w;

    // 恐龍
    dinoImg = new Engine::Image("play/benjamin.png", dinoX, dinoY, DINO_WIDTH, DINO_HEIGHT);
    AddNewObject(dinoImg);

    // 分數顯示
    scoreLabel = new Engine::Label("0", "romulus.ttf", 48, w - 100, 50, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(scoreLabel);

    // 遊戲結束文字
    gameOverLabel = new Engine::Label("GAME OVER", "romulus.ttf", 80, halfW, halfH, 255, 255, 255, 255, 0.5, 0.5);
    gameOverLabel->Visible = false;
    AddNewObject(gameOverLabel);


    gameStartLabel = new Engine::Label("GAME RESTART", "romulus.ttf", 80, halfW, halfH, 255, 255, 255, 255, 0.5, 0.5);
    gameStartLabel->Visible = false;
    AddNewObject(gameStartLabel);

    // 重新開始按鈕
    restartBtn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 100, halfH + 100, 200, 80);
    restartBtn->SetOnClickCallback(std::bind(&LoseScene::RestartGame, this));
    restartBtn->Visible = false;
    AddNewControlObject(restartBtn);

    AddNewObject(new Engine::Label("Get 100pt to restart", "romulus.ttf", 60, halfW, halfH + 140, 0, 0, 0, 255, 0.5, 0.5));

    // 背景音樂
    bgmInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, PlayScene::DangerTime);
}

void LoseScene::Draw() const {
    // 在遊戲結束等待狀態下，繪製特殊畫面

    if (state == GameState::GAME_OVER_WAITING) {
        // 先繪製正常場景作為背景
        IScene::Draw();

        // 獲取屏幕尺寸
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

        // 繪製地面線
        al_draw_line(0, groundY, w, groundY, al_map_rgb(0, 0, 0), 3);

        // 繪製障礙物
        for (const auto& obstacle : obstacles) {
            if (obstacle.type == 0) { // 仙人掌
                al_draw_filled_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 150, 0));
                al_draw_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 0, 0), 2);
            } else { // 鸟
                al_draw_filled_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(200, 0, 0));
                al_draw_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 0, 0), 2);
            }
        }

        // 繪製半透明黑色覆蓋層
        al_draw_filled_rectangle(0, 0, w, h, al_map_rgba(0, 0, 0, 180));

        // 確保遊戲結束標籤可見並繪製
        if (gameOverLabel && gameOverLabel->Visible) {
            gameOverLabel->Draw();
        }
        return;
    }

    if (state == GameState::GAME_START_WAITING) {
        // 先繪製正常場景作為背景
        IScene::Draw();

        // 獲取屏幕尺寸
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

        // 繪製地面線
        al_draw_line(0, groundY, w, groundY, al_map_rgb(0, 0, 0), 3);

        // 繪製障礙物
        for (const auto& obstacle : obstacles) {
            if (obstacle.type == 0) { // 仙人掌
                al_draw_filled_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 150, 0));
                al_draw_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 0, 0), 2);
            } else { // 鸟
                al_draw_filled_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(200, 0, 0));
                al_draw_rectangle(
                    obstacle.x, obstacle.y,
                    obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                    al_map_rgb(0, 0, 0), 2);
            }
        }

        // 繪製半透明黑色覆蓋層
        al_draw_filled_rectangle(0, 0, w, h, al_map_rgba(0, 0, 0, 180));

        // 確保遊戲結束標籤可見並繪製
        if (gameStartLabel && gameStartLabel->Visible) {
            gameStartLabel->Draw();
        }
        return;
    }

    // 正常遊戲狀態下的繪製
    IScene::Draw();

    // 獲取屏幕尺寸
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;

    // 繪製地面線
    al_draw_line(0, groundY, w, groundY, al_map_rgb(0, 0, 0), 3);

    // 繪製障礙物 - 確保在所有UI元素之前繪製
    for (const auto& obstacle : obstacles) {
        if (obstacle.type == 0) { // 仙人掌
            // 使用更醒目的顏色和邊框
            al_draw_filled_rectangle(
                obstacle.x, obstacle.y,
                obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                al_map_rgb(0, 150, 0));

            // 添加黑色邊框讓障礙物更明顯
            al_draw_rectangle(
                obstacle.x, obstacle.y,
                obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                al_map_rgb(0, 0, 0), 2);
        } else { // 鸟
            al_draw_filled_rectangle(
                obstacle.x, obstacle.y,
                obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                al_map_rgb(200, 0, 0));

            // 添加黑色邊框
            al_draw_rectangle(
                obstacle.x, obstacle.y,
                obstacle.x + obstacle.width, obstacle.y + obstacle.height,
                al_map_rgb(0, 0, 0), 2);
        }
    }

    // 調試：顯示障礙物數量（可選，建議在發布版本中移除）
    if (!obstacles.empty()) {
        std::cout << "當前障礙物數量: " << obstacles.size() << std::endl;
        for (size_t i = 0; i < obstacles.size(); ++i) {
            std::cout << "障礙物 " << i << ": x=" << obstacles[i].x
                      << " y=" << obstacles[i].y << " type=" << obstacles[i].type << std::endl;
        }
    }
}


void LoseScene::GenerateObstacle() {
    std::cout << "GenerateObstacle() 被調用" << std::endl;

    Obstacle obstacle;
    obstacle.type = rand() % 2; // 0: 仙人掌, 1: 鸟

    // 获取屏幕宽度作为初始x坐标
    int screenWidth = Engine::GameEngine::GetInstance().GetScreenSize().x;

    if (obstacle.type == 0) {
        obstacle.width = CACTUS_WIDTH;
        obstacle.height = CACTUS_HEIGHT;
        obstacle.y = GROUND_HEIGHT - CACTUS_HEIGHT;
        std::cout << "生成仙人掌" << std::endl;
    } else {
        obstacle.width = BIRD_WIDTH;
        obstacle.height = BIRD_HEIGHT;
        obstacle.y = GROUND_HEIGHT - BIRD_HEIGHT - 70;
        std::cout << "生成鳥" << std::endl;
    }

    obstacle.x = screenWidth;

    obstacles.push_back(obstacle);

    std::cout << "成功添加障礙物到vector，當前數量: " << obstacles.size() << std::endl;
    std::cout << "新障礙物詳情: 類型=" << obstacle.type
              << " 位置=(" << obstacle.x << ", " << obstacle.y << ")"
              << " 尺寸=(" << obstacle.width << ", " << obstacle.height << ")" << std::endl;
}

void LoseScene::Update(float deltaTime) {
    IScene::Update(deltaTime);

    if (state == GameState::GAME_OVER_WAITING) {
        gameOverWaitTime += deltaTime;
        if (gameOverWaitTime >= gameOverWaitDuration) {
            if (!Engine::GameEngine::GetInstance().GetScene("start")) {
                Engine::GameEngine::GetInstance().AddNewScene("start", new StartScene());
            }
            Engine::GameEngine::GetInstance().ChangeScene("start");
        }
        return;
    }
    if (state == GameState::GAME_START_WAITING) {
        gameOverWaitTime += deltaTime;
        if (gameOverWaitTime >= gameOverWaitDuration) {
            if (!Engine::GameEngine::GetInstance().GetScene("play")) {
                Engine::GameEngine::GetInstance().AddNewScene("play", new PlayScene());
            }
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }
        return;
    }

    if (gameOver) return;

    // 添加調試輸出
    static float debugTimer = 0;
    debugTimer += deltaTime;
    if (debugTimer >= 1.0f) {  // 每秒輸出一次
        std::cout << "nextObstacleTime: " << nextObstacleTime
                  << ", deltaTime: " << deltaTime
                  << ", gameSpeed: " << gameSpeed
                  << ", obstacles count: " << obstacles.size() << std::endl;
        debugTimer = 0;
    }

    // 更新分数
    score += deltaTime * gameSpeed;
    if (scoreLabel) {
        scoreLabel->Text = std::to_string((int)score);
    }

    // 检查是否达到100分
    if (score >= 100) {
        state = GameState::GAME_START_WAITING;
    }

    // 更新恐龙位置
    if (isJumping) {
        dinoY += dinoVelocity;
        dinoVelocity += 0.9;

        if (dinoY >= GROUND_HEIGHT - DINO_HEIGHT) {
            dinoY = GROUND_HEIGHT - DINO_HEIGHT;
            isJumping = false;
            dinoVelocity = 0;
        }
    }

    if (isDucking) {
        currentDinoHeight = DINO_HEIGHT * 0.5f;
        ss = 1;
        if (dinoImg) {
            dinoImg->Size.y = currentDinoHeight;
            dinoImg->Position.y = GROUND_HEIGHT - currentDinoHeight;
        }
        dinoY = GROUND_HEIGHT - currentDinoHeight;
    } else {
        if (ss == 1) {
            currentDinoHeight = DINO_HEIGHT;
            if (dinoImg) {
                dinoImg->Size.y = currentDinoHeight;
                dinoImg->Position.y = GROUND_HEIGHT - currentDinoHeight;
            }
            dinoY = GROUND_HEIGHT - currentDinoHeight;
            ss = 0;
        }
    }

    // 更新障碍物位置
    for (auto it = obstacles.begin(); it != obstacles.end(); ) {
        it->x -= gameSpeed;

        if (it->x + it->width < 0) {
            std::cout << "移除障礙物 at x=" << it->x << std::endl;
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }

    // 修正：障礙物生成邏輯
    nextObstacleTime -= deltaTime;  // 直接減去deltaTime，不乘以gameSpeed

    std::cout << "Before check: nextObstacleTime = " << nextObstacleTime << std::endl;

    if (nextObstacleTime <= 0) {
        std::cout << "嘗試生成障礙物..." << std::endl;
        GenerateObstacle();

        // 重設下次生成時間（以秒為單位）
        nextObstacleTime = 1.5f + (rand() % 100) / 100.0f;  // 1.5到2.5秒之間
        std::cout << "已生成障礙物，下次生成時間: " << nextObstacleTime << "秒" << std::endl;
    }

    // 碰撞检测
    for (const auto& obstacle : obstacles) {
        if (CheckCollision(dinoX, dinoY, DINO_WIDTH, currentDinoHeight,
                          obstacle.x+10, obstacle.y, obstacle.width-10, obstacle.height-10)) {
            std::cout << "碰撞檢測到！遊戲結束" << std::endl;
            GameOver();
            break;
        }
    }

    // 更新恐龙图片位置
    if (dinoImg) {
        dinoImg->Position.x = dinoX;
        dinoImg->Position.y = dinoY;
    }

    // 逐渐增加游戏速度
    gameSpeed += deltaTime * 0.01;
}

void LoseScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    if (gameOver) return;

    // 空格鍵或上箭頭跳躍
    if ((keyCode == ALLEGRO_KEY_SPACE || keyCode == ALLEGRO_KEY_UP) && !isJumping) {
        isJumping = true;
        dinoVelocity = -20;
    }
    // 下箭頭蹲下
    else if (keyCode == ALLEGRO_KEY_DOWN) {
        isDucking = true;
        // 這裡可以調整恐龍蹲下的圖片和碰撞框
        dinoVelocity = +15;
    }
}

void LoseScene::OnKeyUp(int keyCode) {
    IScene::OnKeyUp(keyCode);

    if (keyCode == ALLEGRO_KEY_DOWN) {
        isDucking = false;
    }
}

bool LoseScene::CheckCollision(int x1, int y1, int w1, int h1,
                              int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

void LoseScene::GameOver() {
    state = GameState::GAME_OVER_WAITING;
    gameOver = true;
    gameOverWaitTime = 0;

    // 安全地設置UI元素的可見性
    if (gameOverLabel) {
        gameOverLabel->Visible = true;
    }
    if (dinoImg) {
        dinoImg->Visible = false; // 隱藏恐龍
    }
    if (scoreLabel) {
        scoreLabel->Visible = false; // 隱藏分數
    }
    if (restartBtn) {
        restartBtn->Visible = false; // 隱藏重新開始按鈕
    }
}

void LoseScene::RestartGame() {
    // 重置遊戲狀態
    state = GameState::GAME_START_WAITING;
    gameStart = true;
    gameStartWaitTime = 0;

    // 安全地設置UI元素的可見性
    if (gameStartLabel) {
        gameStartLabel->Visible = true;
    }
    if (dinoImg) {
        dinoImg->Visible = false; // 隱藏恐龍
    }
    if (scoreLabel) {
        scoreLabel->Visible = false; // 隱藏分數
    }
    if (restartBtn) {
        restartBtn->Visible = false; // 隱藏重新開始按鈕
    }
}

void LoseScene::Terminate() {
    if (bgmInstance) {
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    IScene::Terminate();
}