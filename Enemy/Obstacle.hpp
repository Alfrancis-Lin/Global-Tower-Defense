//
// Created by peter on 2025/6/11.
//

#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP



#include <allegro5/allegro.h>
#include "Engine/Sprite.hpp"

class Obstacle : public Engine::Sprite {
public:
    int hp;
    Obstacle(std::string img, float x, float y, int hp);
    void Hit(int damage);
};




#endif //OBSTACLE_HPP
