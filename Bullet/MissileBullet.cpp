#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "MissileBullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

MissileBullet::MissileBullet(Engine::Point position, Engine::Point forwardDirection,
                       float rotation, Turret *parent)
    : Bullet("play/bullet-3.png", 1500, 0.5, position, forwardDirection,
             rotation - ALLEGRO_PI / 2, parent)
{
}
void MissileBullet::OnExplode(Enemy *enemy)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect(
        "play/explosion-4.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
