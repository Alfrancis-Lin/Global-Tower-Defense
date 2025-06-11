//
// Created by peter on 2025/6/10.
//

#ifndef BULLET4_HPP
#define BULLET4_HPP



#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet4 : public Bullet {
public:
    explicit Bullet4(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;

};


#endif //BULLET4_HPP
