//
// Created by peter on 25-5-10.
//

#include "FireTurret.hpp"

#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/LaserBullet.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

const int FireTurret::Price = 200;

FireTurret::FireTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-3.png", x, y, 300, Price, 0.5)
{
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void FireTurret::CreateBullet()
{
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2),
                                       sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(
        new LaserBullet(Position + normalized * 36, diff, rotation, this));
    // getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position +
    // normalized * 36 + normal * 6, diff, rotation, this));
    // AudioHelper::PlayAudio("laser.wav");
}

void FireTurret::CreateBullet(Enemy *target)
{
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2),
                                       sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(
        new LaserBullet(Position + normalized * 36, diff, rotation, this));
    // getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position +
    // normalized * 36 + normal * 6, diff, rotation, this));
    // AudioHelper::PlayAudio("laser.wav");
}

void FireTurret::Update(float deltaTime)
{

    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;
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

    // Lock first seen target.
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    Targets.clear();
    for (auto &it : scene->EnemyGroup->GetObjects()) {
        Engine::Point diff = it->Position - Position;
        /*
        if (diff.Magnitude() <= CollisionRadius) {
            Target = dynamic_cast<Enemy *>(it);
            Target->lockedTurrets.push_back(this);
            lockedTurretIterator = std::prev(Target->lockedTurrets.end());
            break;
        }
        */
        if (diff.Magnitude() <= CollisionRadius) {
            Enemy *enemy = dynamic_cast<Enemy *>(it);
            if (enemy) {
                Targets.push_back(enemy);
                if (Targets.size() == 5)
                    break; // 最多 5 個
            }
        }
    }

    if (!Targets.empty()) {

        /*
        Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI /
        2), sin(Rotation - ALLEGRO_PI / 2)); Engine::Point targetRotation =
        (Target->Position - Position).Normalize(); float maxRotateRadian =
        rotateRadian * deltaTime * 3; float cosTheta =
        originRotation.Dot(targetRotation);
        // Might have floating-point precision error.
        if (cosTheta > 1) cosTheta = 1;
        else if (cosTheta < -1) cosTheta = -1;
        float radian = acos(cosTheta);
        Engine::Point rotation;
        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation +
        maxRotateRadian * targetRotation) / radian;
        // Add 90 degrees (PI/2 radian), since we assume the image is oriented
        upward. Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
        */
        // Shoot reload.
        reload -= deltaTime;
        if (reload <= 0.4) {

            reload = coolDown;
            for (auto *target : Targets) {
                Engine::Point targetRotation =
                    (target->Position - Position).Normalize();
                Rotation =
                    atan2(targetRotation.y, targetRotation.x) + ALLEGRO_PI / 2;
                CreateBullet(target); // 對每個目標都射一發
            }
        }
    }
}
