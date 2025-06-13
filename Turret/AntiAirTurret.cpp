//
// Created by 林威佑 on 2025/5/26.
//
#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/Bullet6.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "AntiAirTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/PlaneEnemy.hpp"

const int AntiAirTurret::Price = 100;
AntiAirTurret::AntiAirTurret(float x, float y): Turret("play/tower-base.png", "play/turret-5.png", x, y, 1000, Price, 0.8) {

    Anchor.y += 8.0f / GetBitmapHeight();
}

void AntiAirTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);

    getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 36 , diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 36 + normal * 6, diff, rotation, this));
    //AudioHelper::PlayAudio("gun.wav");
    if(level == 6){
        getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 36 - normal * 3, diff, rotation, this));

        getPlayScene()->BulletGroup->AddNewObject(new Bullet6(Position + normalized * 36 + normal * 3, diff, rotation, this));

    }
}


void AntiAirTurret::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;

    up_cost = 40;

//     if (justPlaced && level == 6 && evo_times) {
//         // std::cout << "Special Effect Activated\n";
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



        if (reload <= 0.3 + 0.04*level) {
            // shoot.
            reload = coolDown;
            CreateBullet();
        }
    }
}
