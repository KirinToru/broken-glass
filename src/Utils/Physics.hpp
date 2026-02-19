#pragma once
#include "VectorOps.hpp"
#include <cmath>

inline float getN(float wl) {
  float l = wl / 1000.f;
  return 1.5046f + 0.012f / (l * l);
}

inline sf::Vector2f refractDir(sf::Vector2f incident, sf::Vector2f normal,
                               float n1, float n2) {
  float cosI = -dot(incident, normal);
  float ratio = n1 / n2;
  float sinT2 = ratio * ratio * (1.f - cosI * cosI);
  if (sinT2 > 1.f)
    return sf::Vector2f(0, 0);
  float cosT = std::sqrt(1.f - sinT2);
  return incident * ratio + normal * (ratio * cosI - cosT);
}
