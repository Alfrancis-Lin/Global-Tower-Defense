#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;

    float hp;
    int money;
    PlayScene *getPlayScene();
    virtual void OnExplode();

public:
    float speed;
    float speedMultiplier = 1;
    float slowTime;
    bool slowing;
    float reachEndTime;
    std::list<Turret *> lockedTurrets;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
    void Hit(float damage);
    virtual void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void Update(float deltaTime) override;

    void Draw() const override;
    virtual bool IsCrossing() const { return false; }
};
#endif   // ENEMY_HPP
