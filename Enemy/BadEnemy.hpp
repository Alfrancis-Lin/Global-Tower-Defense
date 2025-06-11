//
// Created by 林威佑 on 2025/6/10.
//

#ifndef BADENEMY_HPP
#define BADENEMY_HPP
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"

class BadEnemy : public Enemy {
private:
    bool hasDroppedSoldiers = false;
    int sss = 0;
    int tttime = 0;
public:
    BadEnemy(int x, int y);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance) override;
    void Update(float deltaTime) override;
    void DropSoldiers();
    bool IsCrossing() const override;
};
#endif //BADENEMY_HPP
