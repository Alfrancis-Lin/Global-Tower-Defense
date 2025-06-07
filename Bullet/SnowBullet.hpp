#ifndef SNOWBULLET_HPP
#define SNOWBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class SnowBullet : public Bullet {
public:
    explicit SnowBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
    void OnExplode(Enemy *enemy) override;
};
#endif   // SNOWBULLET_HPP
