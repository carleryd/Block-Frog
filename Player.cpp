//
//  Player.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Player.h"


Player::Player(b2World* world) {
    box = new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(400.0f, 100.0f), true); // Size, Position, Density, Friction
}

void Player::draw(sf::RenderWindow* window) {
    window->draw(*box->getShape());
//    std::cout << "Player: x: " << box->getBody()->GetPosition().x << " y: " << box->getBody()->GetPosition().y << std::endl;
    
    box->getShape()->setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
}