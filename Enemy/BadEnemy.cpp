//
// Created by 林威佑 on 2025/6/10.
//
//
// Created by 林威佑 on 2025/5/9.
//

#include <string>
#include <cmath>

#include "BadEnemy.hpp"
#include "Turret/Turret.hpp"
#include "Scene/PlayScene.hpp"

#include "Enemy/SoldierEnemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Enemy/BadEnemy.hpp"
#include "Enemy/BinaryEnemy.hpp"
#include "Enemy/Enemy.hpp"
#include "Enemy/NewEnemy.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "Generator/ProceduralMapGenerator.hpp"
#include "Turret/AntiAirTurret.hpp"
#include "Turret/FreezeTurret.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"
#include "UI/Component/Label.hpp"

BadEnemy::BadEnemy(int x, int y) : Enemy("play/enemy-7.png", x, y, 10, 20, 200, 10) {
    double min = 0.07;
    double max = 0.93;

    /* 產生 [min , max) 的浮點數亂數 */
    double xx = (max - min) * rand() / (RAND_MAX + 1.0) + min;
    Position.x = Engine::GameEngine::GetInstance().GetScreenSize().x*0.8*xx;
    Position.y = -50;

    Velocity = Engine::Point(0, speed);

    CollisionRadius = 20;
}

void BadEnemy::Update(float deltaTime) {
    PlayScene* scene = getPlayScene();

    // 更新位置
    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;

    // 檢查是否還在畫面內
    if (Position.y > Engine::GameEngine::GetInstance().GetScreenSize().y) {
        scene->EnemyGroup->RemoveObject(objectIterator);
        return;
    }

    // 取得當前地圖格子座標
    int gridX = static_cast<int>(Position.x / PlayScene::BlockSize);
    int gridY = static_cast<int>(Position.y / PlayScene::BlockSize);

    // 如果在合法格子內
    if (gridX >= 0 && gridX < PlayScene::MapWidth && gridY >= 0 && gridY < PlayScene::MapHeight) {
        // 取得該格子的砲塔
        if (scene->mapState[gridY][gridX] == PlayScene::TILE_OCCUPIED) {
            scene->mapState[gridY][gridX] = PlayScene::TILE_FLOOR;
            for (auto &it : scene->TowerGroup->GetObjects()) {
                if (it->Position.x == gridX * PlayScene::BlockSize + PlayScene::BlockSize / 2 &&
                    it->Position.y == gridY * PlayScene::BlockSize + PlayScene::BlockSize / 2) {
                    scene->TowerGroup->RemoveObject(it->GetObjectIterator());
                    AudioHelper::PlayAudio("bomb.ogg");
                    break;
                    }
            }
        }

    }

    // 更新旋轉角度
    Rotation = atan2(Velocity.y, Velocity.x);

    // 更新動畫
    Sprite::Update(deltaTime);
}


