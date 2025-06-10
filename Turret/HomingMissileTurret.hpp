#ifndef HOMINGMISSILE_HPP
#define HOMINGMISSILE_HPP
#include "Turret.hpp"

class HomingMissileTurret : public Turret {
public:
    static const int Price;
    HomingMissileTurret(float x, float y);
    void CreateBullet() override;
};
#endif   // HOMINGMISSILE_HPP
