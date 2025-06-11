//
// Created by peter on 2025/6/11.
//

#ifndef COINGEN_HPP
#define COINGEN_HPP



#include "Turret.hpp"

class CoinGen : public Turret {
public:
    static const int Price;

    CoinGen(float x, float y);
    void CreateBullet();
    void Update(float deltaTime) override;
};



#endif //COINGEN_HPP
