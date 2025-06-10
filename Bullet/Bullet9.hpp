//
// Created by peter on 2025/6/10.
//

#ifndef BULLET9_HPP
#define BULLET9_HPP

#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet9 : public Bullet {
public:
    explicit Bullet9(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};



#endif //BULLET9_HPP
