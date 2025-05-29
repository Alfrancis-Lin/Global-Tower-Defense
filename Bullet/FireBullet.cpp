#include <allegro5/base.h>
#include <random>
#include <string>

#include "Engine/Collider.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "FireBullet.hpp"

#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

FireBullet::FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Bullet("play/bullet-1.png", 500, 1, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void FireBullet::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
void FireBullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();

    for (auto &it : scene->EnemyGroup->GetObjects()) {
        if (!it) continue;

        Enemy *enemy = dynamic_cast<Enemy *>(it);


        if (!enemy || !enemy->Visible) continue;



        PlaneEnemy *plane = dynamic_cast<PlaneEnemy *>(enemy);


        if (plane && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
            OnExplode(enemy);
            enemy->Hit(damage);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }



    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2,
                                         Engine::Point(0, 0), PlayScene::GetClientSize())) {
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
        return;}
}

