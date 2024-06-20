#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "constants.hpp"
#include "entity.hpp"

class background : public entity {
  // Private data members
  static sf::Texture& getTexture();
 public:
  
  background(float x, float y);
  background() = default;

  void init(float x, float y) override;

  // Implement the pure virtual functions
  void update() override;
  void draw(sf::RenderWindow& window) override;
};

#endif // BACKGROUND_H
