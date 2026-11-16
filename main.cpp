#include "./solver.h"

#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

void inline fileio(std::ofstream &output, std::ofstream& energy_file, std::size_t iterations, double dt, std::vector<Body> &bodies) {
  double time = 0;

  for (std::size_t i = 0; i < iterations; i++) {
    energy_file << time << ',' << compute_total_energy(bodies) << '\n';
    for (std::size_t j = 0; j < bodies.size(); j++) {
      Body &body = bodies[j];

      output << time << ',' << j << ',' << body.position.x << ',' << body.position.y << '\n';
    }

    time += dt;
    take_step(bodies, dt);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <iterations> <dt>\n"
              << "  iterations: positive integer\n"
              << "  dt: positive, finite timestep\n";

    return 1;
  }

  try {
    // Parse complete arguments before opening (and truncating) the output file.
    const std::string_view iterations_text = argv[1];
    std::size_t iterations{};

    const auto [iterations_end, iterations_error] = std::from_chars(iterations_text.data(), iterations_text.data() + iterations_text.size(), iterations);

    if (iterations_error != std::errc{} || iterations_end != iterations_text.data() + iterations_text.size() || iterations == 0) {
      throw std::invalid_argument("iterations must be a positive integer within the supported range");
    }

    const std::string_view dt_text = argv[2];
    double dt{};

    const auto [dt_end, dt_error] = std::from_chars(dt_text.data(), dt_text.data() + dt_text.size(), dt);

    if (dt_error != std::errc{} || dt_end != dt_text.data() + dt_text.size() || !std::isfinite(dt) || dt <= 0) {
      throw std::invalid_argument("dt must be a positive, finite number within the supported range");
    }

    std::vector<Body> bodies = {
      {1.0, {0.0, 0.0}, {0.0, 0.0}},
      {0.01, {0.0, -1.0}, {0.01, 0.0}}
    };

    std::vector<Vec2> accelerations = compute_acceleration(bodies);

    for (const auto &acceleration : accelerations) {
      std::cout << "Acceleration: " << acceleration.x << ", " << acceleration.y << '\n';
    }

    std::ofstream output;
    output.exceptions(std::ios::failbit | std::ios::badbit);
    output.open("trajectory.csv");

    std::ofstream energy_file;
    energy_file.exceptions(std::ios::failbit | std::ios::badbit);
    energy_file.open("energies.csv");

    output << "time,body_index,x,y\n";
    energy_file << "time,energy\n";

    fileio(output, energy_file, iterations, dt, bodies);

    output.close();

    

    return 0;

  } catch (const std::ios_base::failure &error) {
    std::cerr << "Error opening, writing, or closing trajectory.csv: " << error.what() << '\n';
    return 1;

  } catch (const std::exception &error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
  }
}
