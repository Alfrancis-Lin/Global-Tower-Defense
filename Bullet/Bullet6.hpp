//
// Created by peter on 2025/6/10.
//

#ifndef BULLET6_HPP
#define BULLET6_HPP

#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet6 : public Bullet {
public:
    explicit Bullet6(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};


#endif //BULLET6_HPP
