#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct Ray {
  sf::Color color;
  float wavelength;
  std::vector<sf::Vector2f> points;
  bool valid = false;
};
