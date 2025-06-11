//
// Created by peter on 2025/6/11.
//

#include "Obstacle.hpp"

#include "Engine/Resources.hpp"

Obstacle::Obstacle(std::string img, float x, float y, int hp)
    : Sprite(img, x, y), hp(hp) {}

void Obstacle::Hit(int damage) {
    hp -= damage;
    if (hp <= 0)
        this->Visible = false; // 或者從場景中移除
}