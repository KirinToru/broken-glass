#pragma once
#include "../Utils/VectorOps.hpp"
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cmath>
#include <string>


struct Prism {
  sf::Vector2f pos;
  float size;
  float rotation; // Radians
  std::string name;

  std::array<sf::Vector2f, 3> getVertices() const {
    std::array<sf::Vector2f, 3> v;
    for (int i = 0; i < 3; i++) {
      // Equilateral triangle logic
      float angle = rotation + i * 2 * PI / 3 - PI / 2;
      v[i] = pos + sf::Vector2f(std::cos(angle), std::sin(angle)) * size;
    }
    return v;
  }
};
