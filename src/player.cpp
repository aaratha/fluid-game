#include "player.hpp"

Player::Player(vec2 pos, vec2 targ, float speed, float radius)
    : pos(pos), targ(targ), speed(speed), radius(radius) {}

vec2 Player::getPos() { return pos; }

vec2 Player::getTarg() { return targ; }
float Player::getSpeed() { return speed; }
float Player::getRadius() { return radius; }

void Player::setPos(vec2 pos) { this->pos = pos; }
void Player::setTarg(vec2 targ) { this->targ = targ; }
void Player::setSpeed(float speed) { this->speed = speed; }
void Player::setRadius(float radius) { this->radius = radius; }

void Player::update(float dt, Parameters params) {
  float targSpeed = 10.0;
  vec2 direction = vec2(0, 0);

  // Determine movement direction
  if (IsKeyDown(KEY_W))
    direction += vec2(0, -1);
  if (IsKeyDown(KEY_S))
    direction += vec2(0, 1);
  if (IsKeyDown(KEY_A))
    direction += vec2(-1, 0);
  if (IsKeyDown(KEY_D))
    direction += vec2(1, 0);

  // Normalize the direction if it's not zero
  if (direction.x != 0 || direction.y != 0) {
    direction = Vector2Scale(Vector2Normalize(direction), targSpeed);
  }

  // Update and clamp target
  vec2 newTarg = getTarg() + direction;
  float radius = getRadius();
  newTarg.x =
      std::clamp(newTarg.x, 0.0f, static_cast<float>(params.screenWidth));
  newTarg.y =
      std::clamp(newTarg.y, 0.0f, static_cast<float>(params.screenHeight));
  setTarg(newTarg);

  // Interpolate position towards the clamped target
  setPos(lerp2D(getPos(), getTarg(), getSpeed()));
}
