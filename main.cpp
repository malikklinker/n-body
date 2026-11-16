#include "./solver.h"

#include <charconv>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct SimulationConfig {
  double dt;
  std::size_t iterations;
  std::vector<Body> bodies;
};

SimulationConfig read_config(const std::filesystem::path& path) {
  // First line: iterations dt. Remaining lines: mass x y px py.
  SimulationConfig config{};
  std::ifstream input(path);

  if (!input) {
    throw std::runtime_error("Could not open config file: " + path.string());
  }

  std::string line;
  if (!std::getline(input, line)) {
    throw std::runtime_error("Could not read config header: " + path.string());
  }

  std::istringstream fields(line);
  std::string iterations_text;
  std::string extra;

  if (!(fields >> iterations_text >> config.dt) || fields >> extra) {
    throw std::runtime_error("Line 1: expected iterations dt");
  }

  const auto [end, error] = std::from_chars(iterations_text.data(), iterations_text.data() + iterations_text.size(), config.iterations);

  if (error != std::errc{} || end != iterations_text.data() + iterations_text.size() || config.iterations == 0) {
    throw std::runtime_error("Line 1: iterations must be a positive integer within the supported range");
  }

  if (!std::isfinite(config.dt) || config.dt <= 0) {
    throw std::runtime_error("Line 1: dt must be positive and finite");
  }

  std::size_t line_number = 1;

  while (std::getline(input, line)) {
    ++line_number;
    Body body{};
    std::istringstream body_fields(line);
    const std::string location = "Line " + std::to_string(line_number) + ": ";

    if (!(body_fields >> body.mass >> body.position.x >> body.position.y >> body.momentum.x >> body.momentum.y) || body_fields >> extra) {
      throw std::runtime_error(location + "expected mass x y px py");
    }

    if (!std::isfinite(body.mass) || body.mass <= 0 ||
        !std::isfinite(body.position.x) || !std::isfinite(body.position.y) ||
        !std::isfinite(body.momentum.x) || !std::isfinite(body.momentum.y)) {
      throw std::runtime_error(location + "mass must be positive and all body values must be finite");
    }

    for (const auto& other_body : config.bodies) {
      if (body.position.x == other_body.position.x && body.position.y == other_body.position.y) {
        throw std::runtime_error(location + "body position matches an earlier body");
      }
    }

    config.bodies.push_back(body);
  }

  if (input.bad() || !input.eof()) {
    throw std::runtime_error("Error reading config file: " + path.string());
  }

  if (config.bodies.empty()) {
    throw std::runtime_error("Config must contain at least one body");
  }

  return config;
}

void fileio(std::ofstream& output, std::ofstream& energy_file, std::size_t iterations, double dt, std::vector<Body>& bodies) {
  double time = 0;

  for (std::size_t i = 0; i < iterations; i++) {
    energy_file << time << ',' << compute_total_energy(bodies) << '\n';

    for (std::size_t j = 0; j < bodies.size(); j++) {
      const Body& body = bodies[j];

      output << time << ',' << j << ',' << body.position.x << ',' << body.position.y << '\n';
    }

    if (!output || !energy_file) {
      return;
    }

    time += dt;
    take_step(bodies, dt);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config-file>\n";
    return 1;
  }

  SimulationConfig config{};

  try {
    config = read_config(argv[1]);
  } catch (const std::runtime_error& error) {
    std::cerr << "Config error: " << error.what() << '\n';
    return 1;
  }

  std::ofstream output("trajectory.csv");

  if (!output) {
    std::cerr << "Could not open trajectory.csv for writing\n";
    return 1;
  }

  std::ofstream energy_file("energies.csv");

  if (!energy_file) {
    std::cerr << "Could not open energies.csv for writing\n";
    return 1;
  }

  output << std::setprecision(17) << "time,body_index,x,y\n";
  energy_file << std::setprecision(17) << "time,energy\n";

  fileio(output, energy_file, config.iterations, config.dt, config.bodies);

  output.close();
  energy_file.close();

  bool failed = false;

  if (!output) {
    std::cerr << "Error writing or closing trajectory.csv\n";
    failed = true;
  }

  if (!energy_file) {
    std::cerr << "Error writing or closing energies.csv\n";
    failed = true;
  }

  return failed ? 1 : 0;
}
