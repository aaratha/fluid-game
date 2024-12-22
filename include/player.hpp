#pragma once

#include "utils.hpp"

class Player {
   private:
    vec2 pos;
    vec2 targ;
    float speed;
    float radius;

   public:
    Player(vec2 pos, vec2 targ, float speed, float radius);

    vec2 getPos();
    vec2 getTarg();
    float getSpeed();
    float getRadius();

    void setPos(vec2 pos);
    void setTarg(vec2 targ);
    void setSpeed(float speed);
    void setRadius(float radius);

    void update(float dt, Parameters params);
};
