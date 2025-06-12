#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;

class Turret : public Engine::Sprite {
protected:
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    Sprite imgBase;
    std::list<Turret *>::iterator lockedTurretIterator;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;




public:
    int up_cost;
    int level = 1; // 新增：初始等級
    bool special_effect = false;
    bool Enabled = true;
    bool Preview = false;
    bool justPlaced = false; // 剛放置完成用
    int evo_times = 600;

    Enemy *Target = nullptr;
    Turret(std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown);
    void Update(float deltaTime) override;
    void Draw() const override;
    int GetPrice() const;

    int GetLevel() const { return level; }
    virtual void Upgrade(int newLevel);
    virtual void CreateSpecialBullet(float angle);
    void SetJustPlaced();  // 設置 justPlaced
};
#endif   // TURRET_HPP
