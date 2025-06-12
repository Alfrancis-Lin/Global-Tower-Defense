#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>
#include <iostream>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret.hpp"
#include "Engine/Resources.hpp"
#include "Bullet/LaserBullet.hpp"

PlayScene *Turret::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Turret::Turret(std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown) : Sprite(imgTurret, x, y), price(price), coolDown(coolDown), imgBase(imgBase, x, y) {
    CollisionRadius = radius;
    if(level == 6){
        justPlaced = true;
        special_effect = true;
    }
    //special_effect = true;
    //justPlaced = true;
}
void Turret::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;

    if (justPlaced && level == 6 && evo_times) {
        std::cout << "Special Effect Activated\n";
        const int bulletCount = 360;
        for (int i = 0; i < bulletCount; i++) {
            if(!(evo_times%30)){
                float angle = ALLEGRO_PI * 2 / bulletCount * i;
                CreateSpecialBullet(angle);
            }

        }
        special_effect = false;

        evo_times--;
        if(evo_times == 0) justPlaced = 0;
    }



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



        if (reload <= 0.07*level) {
            // shoot.
            reload = coolDown;
            CreateBullet();
        }
    }
}
void Turret::Draw() const {
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
    if(level == 6)
    al_draw_textf(Engine::Resources::GetInstance().GetFont("romulus.ttf", 30).get(),
                                     al_map_rgb(180, 0, 180),
                                     Position.x,
                                     Position.y + 3, // 砲台底下顯示
                                     ALLEGRO_ALIGN_CENTER,
                                     "MAX", level);

    else
    al_draw_textf(Engine::Resources::GetInstance().GetFont("romulus.ttf", 30).get(),
                   al_map_rgb(255, 255, 255),
                   Position.x,
                   Position.y + 3, // 砲台底下顯示
                   ALLEGRO_ALIGN_CENTER,
                   "Lv%d", level);

}
int Turret::GetPrice() const {
    return price;
}


//升級你的砲台
void Turret::Upgrade(int newLevel) {
    if (newLevel < 1 || newLevel > 6) return; // 限制升級範圍
    level = newLevel;
    // 根據等級改變屬性，以下是舉例：
    if(level >= 1 && level <= 5){
        coolDown = std::max(0.1f, 1.0f - 0.1f * (level - 1)); // 更高等級射速更快
            CollisionRadius += 5 * (level - 1); // 範例：更高等級範圍增加
            // 你也可以根據等級切換不同圖片或其他效果
    }
    else if(level == 6){
        special_effect = true;
        coolDown = std::max(0.1f, 1.1f - 0.1f * (level - 1)); // 更高等級射速更快
        CollisionRadius += 5 * (level - 1); // 範例：更高等級範圍增加
        //BurstEffect();
    }

}

void Turret::CreateSpecialBullet(float angle) {
    Engine::Point dir = Engine::Point(cos(angle), sin(angle));
    float rotation = angle;
    // 子彈起始點可以往外偏移一點（例如 36 像素）：
    getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + dir * 36, dir, rotation, this));
    //AudioHelper::PlayAudio("gun.wav");
}

void Turret::SetJustPlaced() {
    justPlaced = true;
    special_effect = true;

}

