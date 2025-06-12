//
// Created by peter on 2025/6/10.
//
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <allegro5/base.h>
#include <random>
#include <string>
#include "Engine/Collider.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet5.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Obstacle.hpp"

class Turret;

Bullet5::Bullet5(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) : Bullet("play/bullet-5.png", 500, 1, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void Bullet5::OnExplode(Enemy *enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
    if(parent->level == 6) //al_draw_filled_circle(enemy->Position.x, enemy->Position.y, explosionRadius, al_map_rgba(200, 0, 0, 180));
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/explosion-3.png", dist(rng), enemy->Position.x, enemy->Position.y));

}


void Bullet5::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();

    for (auto &it : scene->EnemyGroup->GetObjects()) {
        Enemy *enemy = dynamic_cast<Enemy *>(it);
        if (!enemy->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
            OnExplode(enemy);
            enemy->Hit(damage);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);

            //範圍傷害
            if(parent->level == 6) {
                PlayScene* scene = getPlayScene();
                for (auto& obj : scene->EnemyGroup->GetObjects()) {
                    Enemy* enemy = dynamic_cast<Enemy*>(obj);
                    if (!enemy) continue;
                    Engine::Point diff = enemy->Position - Position;
                    if (diff.Magnitude() <= explosionRadius) {

                        enemy->Hit(damage);

                    }
                }
            }


            return;
        }
    }

    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);

    //PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    for (auto& it : scene->ObstacleGroup->GetObjects()) {
        Obstacle* obs = dynamic_cast<Obstacle*>(it);
        if (!obs->Visible) continue;

        // 碰撞判斷（簡單半徑檢測）
        float dist = (obs->Position - Position).Magnitude();
        if (dist < 30) { // 16 根據子彈/障礙物尺寸微調
            obs->Hit(damage); // 障礙物扣血
            this->Visible = false; // 子彈消失
            return;
        }
    }
}


