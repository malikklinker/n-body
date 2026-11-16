#pragma once

#include <cstddef>
#include <vector>

struct Vec2 {
  double x;
  double y;

  Vec2& operator+=(const Vec2& other) {
   x += other.x;
    y += other.y;
    return *this;
  }
};

inline Vec2 operator+(Vec2 left, const Vec2& right) {
  left += right;
  return left;
}

inline Vec2 operator-(const Vec2& left, const Vec2& right) {
  return {left.x - right.x, left.y - right.y};
}

inline Vec2 operator*(const Vec2& vector, double scalar) {
  return {vector.x * scalar, vector.y * scalar};
}

inline Vec2 operator*(double scalar, const Vec2& vector) {
  return vector * scalar;
}

struct Body {
  double mass;
  Vec2 position;
  Vec2 momentum;
};

double norm(const Vec2& vector_in);
Vec2 body_acceleration(const Body& body, const Body& other_body);
Vec2 compute_acceleration_per_body(const std::vector<Body>& bodies, const Body& body);
std::vector<Vec2> compute_acceleration(const std::vector<Body>& bodies);

void take_step(std::vector<Body> &bodies, double dt);
