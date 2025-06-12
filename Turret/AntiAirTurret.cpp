//
// Created by 林威佑 on 2025/5/26.
//
#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "AntiAirTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/PlaneEnemy.hpp"

const int AntiAirTurret::Price = 150;
AntiAirTurret::AntiAirTurret(float x, float y): Turret("play/tower-base.png", "play/turret-5.png", x, y, 1000, Price, 10) {

    Anchor.y += 8.0f / GetBitmapHeight();
}

void AntiAirTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);

    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 , diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}


void AntiAirTurret::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;

    up_cost = 10;

    if (!Enabled)
        return;


    if (Target) {

        bool validTarget = dynamic_cast<PlaneEnemy*>(Target) &&
                         (Target->Position - Position).Magnitude() <= CollisionRadius;

        if (!validTarget) {
            Target->lockedTurrets.erase(lockedTurretIterator);
            Target = nullptr;
            lockedTurretIterator = std::list<Turret*>::iterator();
        }
    }


    if (!Target) {
        for (auto& it : scene->EnemyGroup->GetObjects()) {
            Engine::Point diff = it->Position - Position;
            if (diff.Magnitude() <= CollisionRadius) {
                if (PlaneEnemy* planeEnemy = dynamic_cast<PlaneEnemy*>(it)) {
                    Target = planeEnemy;
                    Target->lockedTurrets.push_back(this);
                    lockedTurretIterator = std::prev(Target->lockedTurrets.end());
                    break;
                }
            }
        }
    }


    if (Target) {
        Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
        Engine::Point targetRotation = (Target->Position - Position).Normalize();
        float maxRotateRadian = rotateRadian * deltaTime;
        float cosTheta = originRotation.Dot(targetRotation);

        if (cosTheta > 1) cosTheta = 1;
        else if (cosTheta < -1) cosTheta = -1;

        float radian = acos(cosTheta);
        Engine::Point rotation;

        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;

        Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;

        reload -= deltaTime;
        if (reload <= 0) {
            reload = coolDown;
            CreateBullet();
        }
    }
}
