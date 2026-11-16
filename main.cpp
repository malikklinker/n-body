#include <iostream>
#include <vector>
#include "./solver.h"

int main() {
 std::vector<Body> bodies = {
      {1.0,  {0.0,  0.0}, {0.0,  0.0}},
      {0.01, {0.0, -1.0}, {0.01, 0.0}}
  };
  std::vector<Vec2> accelerations = compute_acceleration(bodies);
  for(const auto& acceleration : accelerations){
    std::cout << "Acceleration: " << acceleration.x << ", " << acceleration.y << '\n';
  }
}
