#include "SimpleUI.hpp"
#include <iomanip>
#include <sstream>


SimpleUI::SimpleUI(sf::RenderWindow &win) : window(win) {
  if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
    // Fallback if arial not found (unlikely on Windows)
  }
}

void SimpleUI::begin() {
  sf::Vector2i mp = sf::Mouse::getPosition(window);
  state.mousePos =
      sf::Vector2f(static_cast<float>(mp.x), static_cast<float>(mp.y));
  state.mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  state.hotId = -1;
}

bool SimpleUI::button(int id, const std::string &label, sf::Vector2f pos,
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

bool SimpleUI::slider(int id, const std::string &label, float &value, float min,
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

void SimpleUI::label(const std::string &text, sf::Vector2f pos, int size,
                     sf::Color col) {
  sf::Text t(font, text, size);
  t.setFillColor(col);
  t.setPosition(pos);
  window.draw(t);
}

void SimpleUI::panel(sf::Vector2f pos, sf::Vector2f size) {
  sf::RectangleShape rect(size);
  rect.setPosition(pos);
  rect.setFillColor(sf::Color(30, 30, 30, 200));
  window.draw(rect);
}
