#ifndef FREEZETURRET_HPP
#define FREEZETURRET_HPP
#include "Turret.hpp"



class FreezeTurret : public Turret {
public:
    static const int Price;
    FreezeTurret(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime);
    void Draw() const;
};
#endif   // FREEZETURRET_HPP
