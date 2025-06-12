#include <allegro5/base.h>
#include <string>

#include "BinaryEnemy.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "TankEnemy.hpp"

BinaryEnemy::BinaryEnemy(int x, int y)
    : Enemy("play/enemy-6.png", x, y, 20, 60, 20, 50)
{
}

void BinaryEnemy::Draw() const { Enemy::Draw(); }
void BinaryEnemy::Update(float deltaTime) { Enemy::Update(deltaTime); }

void BinaryEnemy::OnExplode()
{
    // Call base explosion effect
    Enemy::OnExplode();
    // Spawn two SoldierEnemy at this position
    PlayScene *scene = getPlayScene();
    if (scene) {
        Enemy *enemy;
        scene->EnemyGroup->AddNewObject(
            enemy = new TankEnemy(Position.x, Position.y));
        // update it to make it appear on the map
        enemy->UpdatePath(scene->mapDistance);
        enemy->Update(scene->ticks);
        scene->EnemyGroup->AddNewObject(
            enemy = new TankEnemy(Position.x, Position.y));
        // update it to make it appear on the map
        enemy->UpdatePath(scene->mapDistance);
        enemy->Update(scene->ticks);
    }
}
