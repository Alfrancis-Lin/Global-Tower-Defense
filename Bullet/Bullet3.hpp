//
// Created by peter on 2025/6/10.
//

#ifndef BULLET3_HPP
#define BULLET3_HPP

#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet3 : public Bullet {
public:
    explicit Bullet3(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};

#endif //BULLET3_HPP
