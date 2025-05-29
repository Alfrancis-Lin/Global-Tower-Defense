//
// Created by 林威佑 on 2025/5/26.
//

#ifndef ANTIAIRTURRET_HPP
#define ANTIAIRTURRET_HPP

#include "Turret.hpp"

class AntiAirTurret : public Turret {
public:
    static const int Price;
    AntiAirTurret(float x, float y);
    void CreateBullet() override;
    //bool ValidPlacement(int x, int y) const override;
    void Update(float deltaTime) override;
};

#endif //ANTIAIRTURRET_HPP
