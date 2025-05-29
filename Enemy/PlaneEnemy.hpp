//
// Created by 林威佑 on 2025/5/9.
//

#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP
#include "Enemy.hpp"



class PlaneEnemy : public Enemy {
private:
    bool hasDroppedSoldiers = false;
public:
    PlaneEnemy(int x, int y);
    void Update(float deltaTime) override;
    void DropSoldiers();
};


#endif //PLANEENEMY_HPP
