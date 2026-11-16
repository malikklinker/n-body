#include <cmath>
#include <vector>
#include "solver.h"


double norm(const Vec2 &vector_in){
  return std::hypot(vector_in.x, vector_in.y);
}

Vec2 body_acceleration(const Body& body, const Body& other_body) {
  const Vec2 displacement = other_body.position - body.position;
  const double distance = norm(displacement);
  const double scale = other_body.mass / (distance * distance * distance);
  return scale * displacement;
}

Vec2 compute_acceleration_per_body(const std::vector<Body>& bodies, const Body& body){
  Vec2 acceleration;
  acceleration.x = 0;
  acceleration.y = 0;
  for(const auto& other_body : bodies){
    if (&body != &other_body) {
      acceleration+= body_acceleration(body, other_body);
    }
  }
  return acceleration;
}

std::vector<Vec2> compute_acceleration(const std::vector<Body>& bodies){
  std::vector<Vec2> body_accelerations;
  for(const auto& body : bodies){
    body_accelerations.push_back(compute_acceleration_per_body(bodies, body));
  }
  return body_accelerations;
}
void take_step(std::vector<Body> &bodies, double dt) {
  std::vector<Vec2> accelerations_old = compute_acceleration(bodies);

  for (std::size_t i = 0; i < bodies.size(); i++) {
    Body &body = bodies[i];
    body.position = body.position + (dt / body.mass) * body.momentum +
                    (dt * dt / 2) * accelerations_old[i];
  }

  std::vector<Vec2> accelerations_new = compute_acceleration(bodies);

  for (std::size_t i = 0; i < bodies.size(); i++) {
    Body &body = bodies[i];
    body.momentum =
        body.momentum +
        (body.mass * dt / 2) * (accelerations_old[i] + accelerations_new[i]);
  }
}
double compute_total_energy(std::vector<Body> &bodies) {
  double energy = 0;
  for (std::size_t i = 0; i < bodies.size(); i++) {
    Body &body = bodies[i];
    energy += body.momentum * body.momentum / (2 * body.mass);
    for (std::size_t j = 0; j < bodies.size(); j++) {
      if (i < j) {
        Body &body2 = bodies[j];
        energy -= body.mass * body2.mass / norm(body.position - body2.position);
      }
    }
  }
  return energy;
}
