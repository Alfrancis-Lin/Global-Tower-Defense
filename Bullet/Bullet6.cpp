//
// Created by peter on 2025/6/10.
//

#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet6.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

Bullet6::Bullet6(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Bullet("play/arrow.png", 1000, 0.1, position, forwardDirection, rotation + ALLEGRO_PI / 2, parent) {
}
void Bullet6::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}


