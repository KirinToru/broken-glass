#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

const float PI = 3.14159265f;

// --- Physics ---
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

// --- Geometry ---
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

struct LightSource {
  sf::Vector2f pos;
  float angle; // Radians
};

struct Ray {
  sf::Color color;
  float wavelength;
  std::vector<sf::Vector2f> points;
  bool valid = false;
};

// --- UI Framework ---
struct UIState {
  int activeId = -1; // ID of element being interacted with
  int hotId = -1;    // ID of element hovered
  bool mousePressed = false;
  sf::Vector2f mousePos;
};

class SimpleUI {
public:
  SimpleUI(sf::RenderWindow &win) : window(win) {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
      // Fallback if arial not found (unlikely on Windows)
    }
  }

  void begin() {
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    state.mousePos = sf::Vector2f(mp.x, mp.y);
    state.mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    state.hotId = -1;
  }

  bool button(int id, const std::string &label, sf::Vector2f pos,
              sf::Vector2f size) {
    sf::RectangleShape rect(size);
    rect.setPosition(pos);

    bool hovered = rect.getGlobalBounds().contains(state.mousePos);
    if (hovered)
      state.hotId = id;

    bool clicked = false;
    if (state.activeId == id && !state.mousePressed) {
      if (hovered)
        clicked = true;
      state.activeId = -1;
    } else if (hovered && state.mousePressed && state.activeId == -1) {
      state.activeId = id;
    }

    if (state.activeId == id)
      rect.setFillColor(sf::Color(100, 100, 100));
    else if (hovered)
      rect.setFillColor(sf::Color(150, 150, 150));
    else
      rect.setFillColor(sf::Color(200, 200, 200));

    window.draw(rect);

    sf::Text text(font, label, 14);
    text.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({pos.x + (size.x - bounds.size.x) / 2,
                      pos.y + (size.y - bounds.size.y) / 2 - 2});
    window.draw(text);

    return clicked;
  }

  // Returns true if value changed
  bool slider(int id, const std::string &label, float &value, float min,
              float max, sf::Vector2f pos, float width) {
    bool changed = false;
    sf::RectangleShape bar(sf::Vector2f(width, 4));
    bar.setPosition({pos.x, pos.y + 10});
    bar.setFillColor(sf::Color(100, 100, 100));
    window.draw(bar);

    float t = (value - min) / (max - min);
    float handleX = pos.x + t * width;
    sf::CircleShape handle(8);
    handle.setOrigin({8, 8});
    handle.setPosition({handleX, pos.y + 12});

    sf::FloatRect handleRect({handleX - 8, pos.y + 4}, {16, 16});
    bool hovered = handleRect.contains(state.mousePos);
    if (hovered)
      state.hotId = id;

    if (state.activeId == id) {
      if (!state.mousePressed) {
        state.activeId = -1;
      } else {
        float newT = (state.mousePos.x - pos.x) / width;
        if (newT < 0)
          newT = 0;
        if (newT > 1)
          newT = 1;
        value = min + newT * (max - min);
        changed = true;
      }
    } else if (hovered && state.mousePressed && state.activeId == -1) {
      state.activeId = id;
    }

    handle.setFillColor(
        state.activeId == id
            ? sf::Color::White
            : (hovered ? sf::Color(200, 200, 200) : sf::Color(150, 150, 150)));
    window.draw(handle);

    sf::Text lText(font, label, 14);
    lText.setFillColor(sf::Color::White);
    lText.setPosition({pos.x, pos.y - 15});
    window.draw(lText);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value;
    sf::Text vText(font, ss.str(), 14);
    vText.setFillColor(sf::Color::White);
    vText.setPosition({pos.x + width + 10, pos.y});
    window.draw(vText);

    return changed;
  }

  void label(const std::string &text, sf::Vector2f pos, int size = 18,
             sf::Color col = sf::Color::White) {
    sf::Text t(font, text, size);
    t.setFillColor(col);
    t.setPosition(pos);
    window.draw(t);
  }

  void panel(sf::Vector2f pos, sf::Vector2f size) {
    sf::RectangleShape rect(size);
    rect.setPosition(pos);
    rect.setFillColor(sf::Color(30, 30, 30, 200));
    window.draw(rect);
  }

private:
  sf::RenderWindow &window;
  sf::Font font;
  UIState state;
};

// --- Main ---
bool rayLineIntersect(sf::Vector2f origin, sf::Vector2f dir, sf::Vector2f p1,
                      sf::Vector2f p2, sf::Vector2f &hit, float &param) {
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

int main() {
  sf::RenderWindow window(sf::VideoMode({1000, 700}), "Dispersion Simulator");
  SimpleUI ui(window);

  // Scene Objects
  LightSource source = {{50, 350}, 0.1f};
  std::vector<Prism> prisms;
  prisms.push_back({{350, 350}, 150.f, 0.f, "Prism 1"});
  prisms.push_back({{650, 350}, 120.f, PI / 6, "Prism 2"});

  int selectedObjType = 0; // 0=None, 1=Light, 2=Prism
  int selectedPrismIdx = -1;

  // Rays
  const int NUM_RAYS = 50;
  std::vector<Ray> baseRays(NUM_RAYS);
  for (int i = 0; i < NUM_RAYS; i++) {
    float t = (float)i / (NUM_RAYS - 1);
    float wl = 700.f - t * 300.f;
    baseRays[i].wavelength = wl;

    // Color mapping
    if (wl >= 620)
      baseRays[i].color = sf::Color(255, 0, 0);
    else if (wl >= 590) {
      float f = (wl - 590) / 30.f;
      baseRays[i].color = sf::Color(255, (uint8_t)(165 * (1 - f)), 0);
    } else if (wl >= 570)
      baseRays[i].color = sf::Color(255, 255, 0);
    else if (wl >= 495) {
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
    while (const std::optional ev = window.pollEvent()) {
      if (ev->is<sf::Event::Closed>())
        window.close();
    }

    // UI Interaction Start
    ui.begin();

    // --- Update Simulation ---
    std::vector<Ray> rays = baseRays;
    sf::Vector2f srcPos = source.pos;
    sf::Vector2f srcDir =
        sf::Vector2f(std::cos(source.angle), std::sin(source.angle));

    for (auto &ray : rays) {
      ray.points.clear();
      ray.points.push_back(srcPos);

      sf::Vector2f currPos = srcPos;
      sf::Vector2f currDir = srcDir;
      const int MAX_BOUNCES = 10;

      for (int b = 0; b < MAX_BOUNCES; b++) {
        float closestDist = 100000.f;
        sf::Vector2f closestHit, closestNormal;
        bool hitFound = false;

        for (const auto &p : prisms) {
          auto verts = p.getVertices();
          for (int i = 0; i < 3; i++) {
            sf::Vector2f p1 = verts[i];
            sf::Vector2f p2 = verts[(i + 1) % 3];
            sf::Vector2f hit;
            float param;
            if (rayLineIntersect(currPos, currDir, p1, p2, hit, param)) {
              if (len(hit - currPos) < 1.0f)
                continue; // Ignore bias
              float dist = len(hit - currPos);
              if (dist < closestDist) {
                closestDist = dist;
                closestHit = hit;

                sf::Vector2f edge = p2 - p1;
                sf::Vector2f normal = norm(sf::Vector2f(-edge.y, edge.x));
                // Check if normal points out
                sf::Vector2f center = p.pos; // Approximation for equilateral
                sf::Vector2f mid = (p1 + p2) * 0.5f;
                if (dot(normal, mid - center) < 0)
                  normal = -normal; // Ensure outward

                closestNormal = normal;
                hitFound = true;
              }
            }
          }
        }

        if (!hitFound) {
          ray.points.push_back(currPos + currDir * 2000.f);
          ray.valid = true;
          break;
        }

        ray.points.push_back(closestHit);

        // Refraction
        float n_ray = getN(ray.wavelength);
        float n1 = 1.0f, n2 = n_ray;
        sf::Vector2f nVec = closestNormal;

        // Entering or Exiting?
        if (dot(currDir, closestNormal) >
            0) { // Same direction = hitting inside face = Exiting
          n1 = n_ray;
          n2 = 1.0f;
          nVec = -closestNormal; // Invert normal to point against incident
        } else {
          // Hitting outside face = Entering
          n1 = 1.0f;
          n2 = n_ray;
          nVec = closestNormal;
        }

        sf::Vector2f newDir = refractDir(currDir, nVec, n1, n2);
        if (len(newDir) < 0.1f)
          break; // TIR stop

        currDir = norm(newDir);
        currPos = closestHit + currDir * 0.1f; // Bias
      }
    }

    // --- Render ---
    window.clear(sf::Color::Black);

    // Draw Prisms
    for (size_t i = 0; i < prisms.size(); i++) {
      auto v = prisms[i].getVertices();
      sf::VertexArray p(sf::PrimitiveType::LineStrip, 4);
      sf::Color col = (selectedObjType == 2 && selectedPrismIdx == (int)i)
                          ? sf::Color::Yellow
                          : sf::Color::White;
      for (int k = 0; k < 3; k++) {
        p[k].position = v[k];
        p[k].color = col;
      }
      p[3].position = v[0];
      p[3].color = col;
      window.draw(p);
    }

    // Draw Source
    sf::CircleShape srcCircle(8);
    srcCircle.setOrigin({8, 8});
    srcCircle.setPosition(source.pos);
    srcCircle.setFillColor(selectedObjType == 1 ? sf::Color::Yellow
                                                : sf::Color(200, 200, 200));
    window.draw(srcCircle);
    // Source direction hint
    sf::VertexArray dirHint(sf::PrimitiveType::Lines, 2);
    dirHint[0].position = source.pos;
    dirHint[0].color = sf::Color(100, 100, 100);
    dirHint[1].position =
        source.pos +
        sf::Vector2f(std::cos(source.angle), std::sin(source.angle)) * 50.f;
    dirHint[1].color = sf::Color(100, 100, 100);
    window.draw(dirHint);

    // Draw Rays
    // Draw Rays
    for (auto &ray : rays) {
      if (ray.points.size() < 2)
        continue;

      sf::VertexArray line(sf::PrimitiveType::Lines);
      for (size_t i = 0; i < ray.points.size() - 1; i++) {
        sf::Vertex v1;
        v1.position = ray.points[i];

        sf::Vertex v2;
        v2.position = ray.points[i + 1];

        // First segment (Source -> Entry) is white
        if (i == 0) {
          v1.color = sf::Color::White;
          v2.color = sf::Color::White;
        } else {
          v1.color = ray.color;
          v2.color = ray.color;
        }

        line.append(v1);
        line.append(v2);
      }
      window.draw(line);
    }

    // --- Draw UI ---
    // Sidebar background
    ui.panel({800, 0}, {200, 700});
    ui.label("Configuration", {810, 10});

    // Object Selection
    ui.label("Select Object:", {810, 40}, 14);
    if (ui.button(100, "Light Source", {810, 65}, {180, 30})) {
      selectedObjType = 1;
    }
    for (size_t i = 0; i < prisms.size(); i++) {
      std::string label = prisms[i].name;
      if (ui.button(101 + i, label, {810, 105.f + i * 40}, {180, 30})) {
        selectedObjType = 2;
        selectedPrismIdx = i;
      }
    }

    // Properties Panel
    if (selectedObjType == 1) { // Light
      ui.label("Light Properties", {810, 250});
      ui.slider(200, "Pos X", source.pos.x, 0, 800, {810, 280}, 140);
      ui.slider(201, "Pos Y", source.pos.y, 0, 700, {810, 330}, 140);
      float deg = source.angle * 180 / PI;
      if (ui.slider(202, "Angle", deg, -180, 180, {810, 380}, 140)) {
        source.angle = deg * PI / 180.f;
      }

    } else if (selectedObjType == 2 && selectedPrismIdx >= 0 &&
               selectedPrismIdx < (int)prisms.size()) {
      Prism &p = prisms[selectedPrismIdx];
      ui.label("Prism Properties", {810, 250});
      ui.slider(300, "Pos X", p.pos.x, 0, 800, {810, 280}, 140);
      ui.slider(301, "Pos Y", p.pos.y, 0, 700, {810, 330}, 140);
      ui.slider(302, "Size", p.size, 50, 300, {810, 380}, 140);
      float deg = p.rotation * 180 / PI;
      if (ui.slider(303, "Rotation", deg, 0, 360, {810, 430}, 140)) {
        p.rotation = deg * PI / 180.f;
      }
    } else {
      ui.label("No selection", {810, 250}, 14, sf::Color(150, 150, 150));
    }

    // Instructions
    ui.label("Use UI to configure", {810, 650}, 12, sf::Color::Green);

    window.display();
  }

  return 0;
}