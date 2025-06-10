#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/MissileBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "HomingMissileTurret.hpp"
#include "Scene/PlayScene.hpp"

const int HomingMissileTurret::Price = 100;
HomingMissileTurret::HomingMissileTurret(float x, float y)
    : Turret("play/tower-base.png", "play/turret-4.png", x, y, 1400, Price, 0.2) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void HomingMissileTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new MissileBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("gun.wav");
}
