#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Bullet/SnowBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "FreezeTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/Enemy.hpp"

const int FreezeTurret::Price = 50;

FreezeTurret::FreezeTurret(float x, float y)
    : Turret("play/tower-base.png", "play/ice_turret.png", x, y, 200, Price, 1.0) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void FreezeTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new SnowBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}

void FreezeTurret::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;

    CollisionRadius = 150 + 10 * (level - 1);//turret radius upgrade

//     if (justPlaced && level == 6 && evo_times) {
//         //std::cout << "Special Effect Activated\n";
//         const int bulletCount = 360;
//         for (int i = 0; i < bulletCount; i++) {
//             if(!(evo_times%30)){
//                 float angle = ALLEGRO_PI * 2 / bulletCount * i;
//                 CreateSpecialBullet(angle);
//             }
//
//         }
//         special_effect = false;
//
//         evo_times--;
//         if(evo_times == 0) justPlaced = 0;
//     }



    if (!Enabled)
        return;


    if (level == 6) {
        for (auto& obj : scene->EnemyGroup->GetObjects()) {
            Enemy* enemy = dynamic_cast<Enemy*>(obj);
            if (!enemy) continue;
            Engine::Point diff = enemy->Position - Position;
            if (diff.Magnitude() <= CollisionRadius) {
                // 範圍內敵人減速
                enemy->speedMultiplier = 0.5f;
            } else {
                // 不在範圍內的敵人恢復正常速度
                // ⚠️這可能會跟其他 FreezeTurret 衝突，如果有多座要考慮多次計算
                if (enemy->speedMultiplier < 1.0f)
                    enemy->speedMultiplier = 1.0f;
            }
        }
    }

    if (Target) {
        Engine::Point diff = Target->Position - Position;
        if (diff.Magnitude() > CollisionRadius) {
            Target->lockedTurrets.erase(lockedTurretIterator);
            Target = nullptr;
            lockedTurretIterator = std::list<Turret *>::iterator();
        }
    }
    if (!Target) {
        // Lock first seen target.
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        for (auto &it : scene->EnemyGroup->GetObjects()) {
            Engine::Point diff = it->Position - Position;
            if (diff.Magnitude() <= CollisionRadius) {
                Target = dynamic_cast<Enemy *>(it);
                Target->lockedTurrets.push_back(this);
                lockedTurretIterator = std::prev(Target->lockedTurrets.end());
                break;
            }
        }
    }
    if (Target) {
        Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
        Engine::Point targetRotation = (Target->Position - Position).Normalize();
        float maxRotateRadian = rotateRadian * deltaTime;
        float cosTheta = originRotation.Dot(targetRotation);
        // Might have floating-point precision error.
        if (cosTheta > 1) cosTheta = 1;
        else if (cosTheta < -1) cosTheta = -1;
        float radian = acos(cosTheta);
        Engine::Point rotation;
        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
        // Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
        Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
        // Shoot reload.
        reload -= deltaTime;



        if (reload <= 0.01*level) {
            // shoot.
            reload = coolDown;
            CreateBullet();
        }
    }


}

void FreezeTurret::Draw() const {
    if (Preview) {
        al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
    }
    imgBase.Draw();
    Sprite::Draw();
    if (PlayScene::DebugMode) {
        // Draw target radius.
        al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(0, 0, 255), 2);
    }
    // 顯示砲台等級
    al_draw_textf(Engine::Resources::GetInstance().GetFont("romulus.ttf", 30).get(),
                   al_map_rgb(255, 255, 255), // 白色
                   Position.x,
                   Position.y + 7, // 砲台底下顯示
                   ALLEGRO_ALIGN_CENTER,
                   "Lv%d", level);


    if (level == 6) {
        al_draw_filled_circle(Position.x, Position.y, CollisionRadius,
                              al_map_rgba(0, 0, 255, 64)); // 半透明藍色
    }
}