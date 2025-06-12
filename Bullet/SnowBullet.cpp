#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "SnowBullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

SnowBullet::SnowBullet(Engine::Point position, Engine::Point forwardDirection,
                       float rotation, Turret *parent)
    : Bullet("play/ice_bullet.png", 500, 0.3, position, forwardDirection,
             rotation - ALLEGRO_PI / 2, parent)
{
}
void SnowBullet::OnExplode(Enemy *enemy)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    enemy->slowTime = parent->level;
    enemy->slowing = true;
    enemy->Tint = al_map_rgba(0, 0, 155, 205);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect(
        "play/ice_dirty.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
