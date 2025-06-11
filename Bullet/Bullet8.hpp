//
// Created by peter on 2025/6/10.
//

#ifndef BULLET8_HPP
#define BULLET8_HPP

#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet8 : public Bullet {
public:
    explicit Bullet8(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};



#endif //BULLET8_HPP
