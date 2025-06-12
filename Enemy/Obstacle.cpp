//
// Created by peter on 2025/6/11.
//

#include "Obstacle.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"

Obstacle::Obstacle(std::string img, float x, float y, int hp, int gridX, int gridY)
    : Sprite(img, x, y), hp(hp), GridX(gridX), GridY(gridY) {}


void Obstacle::Hit(int damage) {
    hp -= damage;
    if (hp <= 0){
        this->Visible = false; // 或者從場景中移除
        auto scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
                if (scene) {
                    scene->mapState[GridY][GridX] = PlayScene::TILE_FLOOR;
                }
    }


}