#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdint>

constexpr float PI = 3.14159265f;

inline float len(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

inline sf::Vector2f norm(sf::Vector2f v) {
  float l = len(v);
  return l > 0.001f ? v / l : sf::Vector2f(1, 0);
}

inline float dot(sf::Vector2f a, sf::Vector2f b) {
  return a.x * b.x + a.y * b.y;
}

inline bool rayLineIntersect(sf::Vector2f origin, sf::Vector2f dir,
                             sf::Vector2f p1, sf::Vector2f p2,
                             sf::Vector2f &hit, float &param) {
  sf::Vector2f edge = p2 - p1;
  float denom = dir.x * edge.y - dir.y * edge.x;
  if (std::abs(denom) < 0.001f)
    return false;

  sf::Vector2f diff = origin - p1;
  float t = (diff.y * edge.x - diff.x * edge.y) / denom;
  float s = (diff.y * dir.x - diff.x * dir.y) / denom;

  if (t > 0.01f && s >= 0.f && s <= 1.f) {
    hit = p1 + edge * s;
    param = s;
    return true;
  }
  return false;
}
