//
// Created by 林威佑 on 2025/5/9.
//

#include <string>

#include "PlaneEnemy.hpp"


#include "Enemy/SoldierEnemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

PlaneEnemy::PlaneEnemy(int x, int y) : Enemy("play/enemy-2.png", x, y, 10, 20, 2, 10) {

    Position.x = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    Position.y = -50;

    Velocity = Engine::Point(0, speed);

    CollisionRadius = 20;
}

void PlaneEnemy::Update(float deltaTime) {

    int x = static_cast<int>(Position.x / PlayScene::BlockSize);
    int y = static_cast<int>(Position.y / PlayScene::BlockSize);

    if (x >= 0 && x < PlayScene::MapWidth && y >= 0 && y < PlayScene::MapHeight) {
        PlayScene* scene = getPlayScene();
        if (scene->mapState[y][x] == PlayScene::TILE_DIRT && !hasDroppedSoldiers) {
            DropSoldiers();
            hasDroppedSoldiers = true;
        }
        if (scene->mapState[y][x] == PlayScene::TILE_FLOOR && hasDroppedSoldiers) {

            hasDroppedSoldiers = false;
        }
    }
    Rotation = atan2(Velocity.y, Velocity.x);

    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;


    if (Position.y > Engine::GameEngine::GetInstance().GetScreenSize().y ) {
        getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
    }

    Sprite::Update(deltaTime);
}

void PlaneEnemy::DropSoldiers() {
    PlayScene* scene = getPlayScene();

    for (int i = 0; i < 5; i++) {
        float offsetX = (i - 1) * 20; // -20, 0, 20 offset
        Enemy* soldier = new SoldierEnemy(Position.x + offsetX, Position.y);
        scene->EnemyGroup->AddNewObject(soldier);
        soldier->UpdatePath(scene->mapDistance);
    }
}