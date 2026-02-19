#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>


struct UIState {
  int activeId = -1; // ID of element being interacted with
  int hotId = -1;    // ID of element hovered
  bool mousePressed = false;
  sf::Vector2f mousePos;
};

class SimpleUI {
public:
  SimpleUI(sf::RenderWindow &win);

  void begin();

  bool button(int id, const std::string &label, sf::Vector2f pos,
              sf::Vector2f size);

  // Returns true if value changed
  bool slider(int id, const std::string &label, float &value, float min,
              float max, sf::Vector2f pos, float width);

  void label(const std::string &text, sf::Vector2f pos, int size = 18,
             sf::Color col = sf::Color::White);

  void panel(sf::Vector2f pos, sf::Vector2f size);

private:
  sf::RenderWindow &window;
  sf::Font font;
  UIState state;
};
