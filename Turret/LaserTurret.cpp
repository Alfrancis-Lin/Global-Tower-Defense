#include <allegro5/base.h>
#include <cmath>
#include <string>
#include "Bullet/Bullet5.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserTurret.hpp"
#include "Scene/PlayScene.hpp"

const int LaserTurret::Price = 20;
LaserTurret::LaserTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-2.png", x, y, 170, Price, 0.8) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    up_cost = 40;
}
void LaserTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new Bullet5(Position + normalized * 36 - normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new Bullet5(Position + normalized * 36 + normal * 6, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}
