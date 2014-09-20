//
//  Player.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Player.h"


Player::Player(b2World* world_, sf::RenderWindow* w) {
    world = world_;
    box = new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(0, 0), w, true); // Size, Position, Density, Friction
	box->getShape()->setFillColor(sf::Color(0, 255, 0));
	jumpHeight = 10;
}

void Player::draw(sf::RenderWindow* window) {
    window->draw(*box->getShape());
    
    box->getShape()->setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
}

void Player::setPosition(b2Vec2* newPos) {
	box->getBody()->SetTransform(b2Vec2(newPos->x, newPos->y), box->getBody()->GetAngle());
}

void Player::move(int dir)
{
	switch (dir)
	{
	case LEFT:
            leftSpeed = -10;
		break;
	case RIGHT:
			rightSpeed = 10;
		break;
    case LEFT_STOP:
            leftSpeed = 0;
        break;
    case RIGHT_STOP:
            rightSpeed = 0;
        break;
	case JUMP:
		push(b2Vec2(0, jumpHeight));
		break;
	default:
		break;
	}
}

void Player::isJumping() {
//    std::vector<b2Contact> contactList = world->GetContactList();
	b2Contact* contact = world->GetContactList();
//    std::cout << world->GetContactCount() << std::endl;
	
    while(contact != nullptr) {
        if(contact->GetFixtureA() == box->getBody()->GetFixtureList())
            std::cout << "COLLISION" << std::endl;
		
        contact = contact->GetNext();
    }
}

void Player::push(b2Vec2&& dir)
{
	box->getBody()->ApplyLinearImpulse(dir, box->getBody()->GetPosition(), true);
}

void Player::updatePlayer()
{
    isJumping();
	b2Vec2 oldSpeed = box->getBody()->GetLinearVelocity();
    oldSpeed = b2Vec2(0, oldSpeed.y);
	box->update();
    box->getBody()->SetLinearVelocity(b2Vec2(leftSpeed + rightSpeed, 0) + oldSpeed);
}