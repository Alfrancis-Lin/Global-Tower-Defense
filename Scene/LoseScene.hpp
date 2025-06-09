#ifndef LOSESCENE_HPP
#define LOSESCENE_HPP

#include "Engine/IScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>  // 添加基本繪圖功能
#include <memory>
#include <vector>

class PlayScene;  // 前向聲明

class LoseScene final : public Engine::IScene {
private:
    // 恐龍遊戲相關變數
    struct Obstacle {
        int x, y;
        int width, height;
        int type; // 0: 仙人掌, 1: 鳥
    };

    int dinoX, dinoY;
    float dinoVelocity;
    bool isJumping;
    bool isDucking;
    float gameSpeed;
    float score;
    bool gameOver;
    std::vector<Obstacle> obstacles;
    float nextObstacleTime;
    int groundY;
    float groundX1, groundX2;

    // 遊戲物件
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    Engine::Image* dinoImg;
    Engine::Label* scoreLabel;
    Engine::Label* gameOverLabel;
    Engine::ImageButton* restartBtn;

    // 遊戲方法
    void GenerateObstacle();
    bool CheckCollision(int x1, int y1, int w1, int h1,
                       int x2, int y2, int w2, int h2);
    void GameOver();
    void RestartGame();

public:
    explicit LoseScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void BackOnClick(int stage);
};

#endif // LOSESCENE_HPP