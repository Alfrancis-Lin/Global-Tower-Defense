//
// Created by peter on 2025/6/10.
//

#ifndef BULLET7_HPP
#define BULLET7_HPP

#include "Bullet.hpp"
class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet7 : public Bullet {
public:
    explicit Bullet7(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};


#endif //BULLET7_HPP
