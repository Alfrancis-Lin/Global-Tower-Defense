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
#include "Bullet9.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

Bullet9::Bullet9(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Bullet("play/fire_bullet.png", 1000, 0.02, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void Bullet9::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    //getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}


