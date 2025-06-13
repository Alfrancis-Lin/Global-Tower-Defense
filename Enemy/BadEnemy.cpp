#include <cmath>
#include <string>

#include "BadEnemy.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"

#include "Enemy/BadEnemy.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/TurretButton.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/Plane.hpp"

BadEnemy::BadEnemy(int x, int y)
    : Enemy("play/enemy-7.png", x, y, 10, 20, 20, 10)
{

    double min = 0.07;
    double max = 0.93;

    /* 產生 [min , max) 的浮點數亂數 */

    double xx = (max - min) * rand() / (RAND_MAX + 1.0) + min;
    Position.x = Engine::GameEngine::GetInstance().GetScreenSize().x * 0.8 * xx;
    Position.y = -50;

    Velocity = Engine::Point(0, speed);

    CollisionRadius = 25;
    sss = 0;
    tttime = 0;
}
void BadEnemy::UpdatePath(const std::vector<std::vector<int>> &mapDistance)
{

}

void BadEnemy::Update(float deltaTime)
{
    PlayScene *scene = getPlayScene();

    // 更新位置
    if (sss==1) {
        tttime++;
    }
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
    if (gridX >= 0 && gridX < PlayScene::MapWidth && gridY >= 0 &&
        gridY < PlayScene::MapHeight) {
        // 取得該格子的砲塔

        if (scene->mapState[gridY][gridX] == PlayScene::TILE_OCCUPIED) {
            scene->mapState[gridY][gridX] = PlayScene::TILE_FLOOR;
            for (auto &it : scene->TowerGroup->GetObjects()) {
                if (it->Position.x == gridX * PlayScene::BlockSize +
                                          (double)PlayScene::BlockSize / 2 &&
                    it->Position.y == gridY * PlayScene::BlockSize +
                                          (double)PlayScene::BlockSize / 2 && sss==0) {

                    AudioHelper::PlayAudio("bomb.ogg");
                    sss = 1;
                    Velocity.x = 0;
                    Velocity.y = 0;

                    //tttime++;
                    // std::cout<<tttime;
                    if (tttime >= 300) {
                        scene->TowerGroup->RemoveObject(
                            it->GetObjectIterator());
                        scene->mapState[gridY][gridX] = PlayScene::TILE_FLOOR;
                        Enemy::OnExplode();
                        Enemy::Hit(10000.0);
                        // AudioHelper::PlayAudio("bomb.ogg");
                        break;
                    }
                }
            }
        }
    }
    for (auto &it : scene->TowerGroup->GetObjects())
    {
        if (it->Position.x == gridX * PlayScene::BlockSize +
                                  (double)PlayScene::BlockSize / 2 &&
            it->Position.y == gridY * PlayScene::BlockSize +
                                  (double)PlayScene::BlockSize / 2 &&sss==1)
            {

            if (tttime >= 420) {
                scene->TowerGroup->RemoveObject(
                    it->GetObjectIterator());
                scene->mapState[gridY][gridX] = PlayScene::TILE_FLOOR;
                Enemy::OnExplode();
                Enemy::Hit(10000.0);
                // AudioHelper::PlayAudio("bomb.ogg");
                break;
            }
            }

    }
    // 更新旋轉角度
    Rotation = atan2(Velocity.y, Velocity.x);

    // 更新動畫
    Sprite::Update(deltaTime);
}

bool BadEnemy::IsCrossing() const {
    return true;
}
