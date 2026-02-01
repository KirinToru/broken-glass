#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

const float PI = 3.14159265f;

// Cauchy's equation with INCREASED dispersion
float getN(float wl) {
  float l = wl / 1000.f;
  return 1.5046f + 0.012f / (l * l);
}

float len(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }
sf::Vector2f norm(sf::Vector2f v) {
  float l = len(v);
  return l > 0.001f ? v / l : sf::Vector2f(1, 0);
}
float dot(sf::Vector2f a, sf::Vector2f b) { return a.x * b.x + a.y * b.y; }

// Refract direction using Snell's law
sf::Vector2f refractDir(sf::Vector2f incident, sf::Vector2f normal, float n1,
                        float n2) {
  float cosI = -dot(incident, normal);
  float ratio = n1 / n2;
  float sinT2 = ratio * ratio * (1.f - cosI * cosI);
  if (sinT2 > 1.f)
    return sf::Vector2f(0, 0);
  float cosT = std::sqrt(1.f - sinT2);
  return incident * ratio + normal * (ratio * cosI - cosT);
}

// Ray-line intersection
bool rayLineIntersect(sf::Vector2f origin, sf::Vector2f dir, sf::Vector2f p1,
                      sf::Vector2f p2, sf::Vector2f &hit, float &param) {
  sf::Vector2f edge = p2 - p1;
  float denom = dir.x * edge.y - dir.y * edge.x;
  if (std::abs(denom) < 0.001f)
    return false;

  sf::Vector2f diff = origin - p1;
  float t = (diff.y * edge.x - diff.x * edge.y) / denom;
  float s = (diff.y * dir.x - diff.x * dir.y) / denom;

  if (t > 0.001f && s >= 0.f && s <= 1.f) {
    hit = p1 + edge * s;
    param = s;
    return true;
  }
  return false;
}

struct Ray {
  sf::Color color;
  float wavelength;
  sf::Vector2f entry, exit, end;
  bool ok = false;
};

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}),
                          "Dispersion - Use mouse to aim light");

  // Prism vertices
  sf::Vector2f A(400, 100), B(200, 500), C(600, 500);

  // Normals
  sf::Vector2f leftN = norm(sf::Vector2f(-(B - A).y, (B - A).x));
  sf::Vector2f rightN = norm(sf::Vector2f(-(C - A).y, (C - A).x));

  // Light source - moveable
  sf::Vector2f source(50, 260);
  sf::Vector2f target(300, 300); // Where light is aimed

  // Spectrum wavelengths
  const int NUM_RAYS = 30;
  std::vector<Ray> baseRays(NUM_RAYS);
  for (int i = 0; i < NUM_RAYS; i++) {
    float t = (float)i / (NUM_RAYS - 1);
    float wl = 700.f - t * 300.f;
    baseRays[i].wavelength = wl;

    // Color based on wavelength
    if (wl >= 620) {
      baseRays[i].color = sf::Color(255, 0, 0);
    } else if (wl >= 590) {
      float f = (wl - 590) / 30.f;
      baseRays[i].color = sf::Color(255, (uint8_t)(165 * (1 - f)), 0);
    } else if (wl >= 570) {
      baseRays[i].color = sf::Color(255, 255, 0);
    } else if (wl >= 495) {
      float f = (wl - 495) / 75.f;
      baseRays[i].color = sf::Color((uint8_t)(255 * f), 255, 0);
    } else if (wl >= 450) {
      float f = (wl - 450) / 45.f;
      baseRays[i].color = sf::Color(0, (uint8_t)(255 * f), 255);
    } else {
      float f = (wl - 400) / 50.f;
      baseRays[i].color =
          sf::Color((uint8_t)(148 * (1 - f)), 0, (uint8_t)(211 + 44 * f));
    }
  }

  while (window.isOpen()) {
    // Event handling
    while (const std::optional ev = window.pollEvent()) {
      if (ev->is<sf::Event::Closed>())
        window.close();

      // Move source with arrow keys
      if (ev->is<sf::Event::KeyPressed>()) {
        auto key = ev->getIf<sf::Event::KeyPressed>();
        float speed = 10.f;
        if (key->code == sf::Keyboard::Key::Up)
          source.y -= speed;
        if (key->code == sf::Keyboard::Key::Down)
          source.y += speed;
        if (key->code == sf::Keyboard::Key::Left)
          source.x -= speed;
        if (key->code == sf::Keyboard::Key::Right)
          source.x += speed;
      }

      // Aim with mouse
      if (ev->is<sf::Event::MouseMoved>()) {
        auto mouse = ev->getIf<sf::Event::MouseMoved>();
        target =
            sf::Vector2f((float)mouse->position.x, (float)mouse->position.y);
      }
    }

    // Calculate ray direction
    sf::Vector2f inDir = norm(target - source);

    // Find entry point on prism (left edge A-B)
    sf::Vector2f entry;
    float param;
    bool hasEntry = rayLineIntersect(source, inDir, A, B, entry, param);

    // Calculate rays
    std::vector<Ray> rays = baseRays;
    if (hasEntry) {
      for (auto &r : rays) {
        r.entry = entry;
        r.ok = false;

        float n = getN(r.wavelength);

        // Refract at entry
        sf::Vector2f intDir = refractDir(inDir, leftN, 1.f, n);
        if (len(intDir) < 0.5f)
          continue;
        intDir = norm(intDir);

        // Find exit on right edge (A-C)
        sf::Vector2f exitPoint;
        float exitParam;
        if (rayLineIntersect(entry, intDir, A, C, exitPoint, exitParam)) {
          r.exit = exitPoint;

          // Refract at exit
          sf::Vector2f exitDir = refractDir(intDir, rightN, n, 1.f);
          if (len(exitDir) < 0.5f)
            continue;
          exitDir = norm(exitDir);

          r.end = r.exit + exitDir * 300.f;
          r.ok = true;
        }
      }
    }

    // Render
    window.clear(sf::Color::Black);

    // Draw prism
    sf::VertexArray prism(sf::PrimitiveType::LineStrip, 4);
    prism[0].position = A;
    prism[1].position = B;
    prism[2].position = C;
    prism[3].position = A;
    for (int i = 0; i < 4; i++)
      prism[i].color = sf::Color::White;
    window.draw(prism);

    // Draw source indicator
    sf::CircleShape srcCircle(5);
    srcCircle.setFillColor(sf::Color::Yellow);
    srcCircle.setPosition(source - sf::Vector2f(5, 5));
    window.draw(srcCircle);

    // Draw white incident ray
    if (hasEntry) {
      sf::VertexArray w(sf::PrimitiveType::Lines, 2);
      w[0].position = source;
      w[0].color = sf::Color::White;
      w[1].position = entry;
      w[1].color = sf::Color::White;
      window.draw(w);
    } else {
      // Draw ray going to target if no hit
      sf::VertexArray w(sf::PrimitiveType::Lines, 2);
      w[0].position = source;
      w[0].color = sf::Color(100, 100, 100);
      w[1].position = source + inDir * 500.f;
      w[1].color = sf::Color(100, 100, 100);
      window.draw(w);
    }

    // Draw colored rays
    for (auto &r : rays) {
      if (r.ok) {
        sf::VertexArray in(sf::PrimitiveType::Lines, 2);
        in[0].position = r.entry;
        in[0].color = r.color;
        in[1].position = r.exit;
        in[1].color = r.color;
        window.draw(in);

        sf::VertexArray out(sf::PrimitiveType::Lines, 2);
        out[0].position = r.exit;
        out[0].color = r.color;
        out[1].position = r.end;
        out[1].color = r.color;
        window.draw(out);
      }
    }

    window.display();
  }

  return 0;
}