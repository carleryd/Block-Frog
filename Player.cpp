//
//  Player.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Player.h"


Player::Player(b2World* world, sf::RenderWindow* w) {
    box = new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(0, 0), w, true); // Size, Position, Density, Friction
	box->getShape()->setFillColor(sf::Color(0, 255, 0));
}

void Player::draw(sf::RenderWindow* window) {
    window->draw(*box->getShape());
//    std::cout << "Player: x: " << box->getBody()->GetPosition().x << " y: " << box->getBody()->GetPosition().y << std::endl;
    
    box->getShape()->setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
}

void Player::setPosition(b2Vec2* newPos) {
	box->getBody()->SetTransform(b2Vec2(newPos->x, newPos->y), box->getBody()->GetAngle());
}

void Player::move(b2Vec2&& dir)
{
	box->getBody()->ApplyLinearImpulse(dir, box->getBody()->GetPosition(), true);
}

void Player::updatePlayer()
{
	
	box->update();
}