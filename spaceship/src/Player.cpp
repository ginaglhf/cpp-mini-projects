//
// Created by yingbingzhu on 2025/3/20.
//

#include "../headers/Player.h"

Player::Player(): sprite(texture), position(400.f, 500.f) {
  // Load the texture in the constructor body
  if (!texture.loadFromFile("resources/spaceship.png")) {
    std::cerr << "Error loading player texture!" << std::endl;
    // Fallback: Set a color and size for the sprite if texture fails to load
    sprite.setColor(sf::Color::Red);
    sprite.setTextureRect(sf::IntRect({0, 0}, {50, 50}));  // Dummy rectangle for visibility
  } else {
    // Texture loaded successfully
    sprite.setTexture(texture, true);  // Reset texture rect to texture size

    // Get the original size of the sprite
    sf::FloatRect spriteBounds = sprite.getLocalBounds();

    // Desired size (e.g., 64x64 pixels)
    constexpr float desiredSize = 64.f;
    float scaleX = desiredSize / spriteBounds.size.x;
    float scaleY = desiredSize / spriteBounds.size.y;

    // Apply the scale to the sprite
    sprite.setScale({scaleX, scaleY});

    // Recalculate the bounds after scaling (for setting the origin)
    spriteBounds = sprite.getLocalBounds();
    sprite.setOrigin({spriteBounds.size.x / 2.f, spriteBounds.size.y / 2.f});
  }

  // Set the sprite's position
  sprite.setPosition(position);

  // Health bar background
  healthBarBack.setSize({healthBarWidth, healthBarHeight});
  healthBarBack.setFillColor(sf::Color(50, 50, 50));
  healthBarBack.setPosition({10.f, 10.f});

  // Health bar fill
  healthBarFront.setSize({healthBarWidth, healthBarHeight});
  healthBarFront.setFillColor(sf::Color::Red);
  healthBarFront.setPosition(healthBarBack.getPosition());

}

void Player::update(const float deltaTime) {
  sf::Vector2f movement(0.f, 0.f);
  if (isKeyPressed(sf::Keyboard::Key::Left) || isKeyPressed(sf::Keyboard::Key::A))
      movement.x -= speed;
  if (isKeyPressed(sf::Keyboard::Key::Right) || isKeyPressed(sf::Keyboard::Key::D))
    movement.x += speed;
  if (isKeyPressed(sf::Keyboard::Key::Up) || isKeyPressed(sf::Keyboard::Key::W))
    movement.y -= speed;
  if (isKeyPressed(sf::Keyboard::Key::Down) || isKeyPressed(sf::Keyboard::Key::S))
    movement.y += speed;
  movement *= deltaTime;
  position += movement;
  sprite.setPosition(position);
};

void Player::processShooting(const float deltaTime) {
  if (shootCooldown > 0.f)
    shootCooldown -= deltaTime;
  if (isKeyPressed(sf::Keyboard::Key::Space) && shootCooldown <= 0.f) {
    const sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f bulletPosition(bounds.position.x + bounds.size.x / 2.f - 5.f, bounds.position.y);
    bullets.emplace_back(bulletPosition);

    // Reset the cooldown
    shootCooldown = shootCooldownTime;;
  }
};


void Player::updateBullets(const float deltaTime) {
  // Update the position of each bullet.
  for (auto& bullet : bullets) {
    if (bullet.isActive()) {
      bullet.update(deltaTime);
      if (bullet.shape.getPosition().y < 0) {
        bullet.deactivate();
      }
    }
  }

  // Remove bullets that have moved off-screen (above the window).
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet &b) {
      return b.shape.getPosition().y + b.shape.getRadius() * 2 < 0;
  }), bullets.end());
}

void Player::drawBullets(sf::RenderWindow& window) const {
  for (auto& bullet : bullets) {
    if (bullet.isActive()) {
      window.draw(bullet.shape);
    }
  }
}

void Player::deductHealth(const int amount) {
  health -= amount;
  if (health < 0) {
    health = 0;
  }
  std::cout << "Player health: " << health << std::endl;
}

void Player::draw(sf::RenderWindow& window) {
  window.draw(sprite);
  // Calculate fill ratio
  float ratio = static_cast<float>(health) / maxHealth;
  healthBarFront.setSize({ healthBarWidth * ratio, healthBarHeight });

  // Draw UI on top
  window.draw(healthBarBack);
  window.draw(healthBarFront);

  drawBullets(window);
}


