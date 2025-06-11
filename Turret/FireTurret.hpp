//
// Created by peter on 25-5-10.
//

#ifndef FIRETURRET_H
#define FIRETURRET_H

#include "Turret.hpp"
#include <vector>

class FireTurret : public Turret {
  public:
    static const int Price;
    FireTurret(float x, float y);
    void CreateBullet(Enemy *);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    std::vector<Enemy *> Targets;
};

#endif // FIRETURRET_H
