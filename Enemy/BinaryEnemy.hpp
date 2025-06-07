#ifndef BINARYENEMY_HPP
#define BINARYENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class BinaryEnemy : public Enemy {
public:
    BinaryEnemy(int x, int y);
    void Draw() const override;
    void Update(float deltaTime) override;
    void OnExplode() override;
};
#endif   // BINARYENEMY_HPP
