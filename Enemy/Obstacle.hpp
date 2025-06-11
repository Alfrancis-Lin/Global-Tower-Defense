//
// Created by peter on 2025/6/11.
//

#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP


#include "Scene/PlayScene.hpp"
#include <allegro5/allegro.h>
#include "Engine/Sprite.hpp"

class Obstacle : public Engine::Sprite {
public:
    int hp;
    int GridX, GridY;
    Obstacle(std::string img, float x, float y, int hp, int gridX, int gridY);
    void Hit(int damage);
    PlayScene *getPlayScene();
};




#endif //OBSTACLE_HPP
