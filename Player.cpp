//
//  Player.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Player.h"
#include "Game.h"
#include "OSHandler.h"


Player::Player(Game* game_) {
    game = game_;
    world = game->getWorld();
    window = game->getWindow();
    
    box = new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(0, 0), window, true); // Size, Position, Density, Friction
//	box->getShape()->setFillColor(sf::Color(0, 255, 0));
    
//    box->getShape()->setTexture(frogTexture);
	jumpHeight = 50;
    
    if (!frogTexture.loadFromFile(game->getOSHandler()->getResourcePath() + "frog_placeholder.png")) {
        std::cout << "Could not load frog image" << std::endl;
    }
    
    frogSprite.setTexture(frogTexture);
    frogSprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
    frogSprite.setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
    
//    frogSprite.rotate(30);
//    std::cout << box->getBody()->GetPosition().x << " " << box->getBody()->GetPosition().y << std::endl;
    
    // Foot sensor
    b2PolygonShape polygonShape;
    
    b2FixtureDef myFixtureDef;
    myFixtureDef.shape = &polygonShape;
    myFixtureDef.density = 1;
    
    polygonShape.SetAsBox(0.3, 0.3, b2Vec2(0,-2), 0);
    myFixtureDef.isSensor = true;
    b2Fixture* footSensorFixture = box->getBody()->CreateFixture(&myFixtureDef);
    footSensorFixture->SetUserData( (void*)3 );
    
    contactListener = new ContactListener();
    world->SetContactListener(contactListener);
}

void Player::draw(sf::RenderWindow* window) {
//    window->draw(*box->getShape());
    window->draw(frogSprite);
    
    frogSprite.setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
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
            if(!isJumping())
				push(b2Vec2(0, jumpHeight));
		break;
	default:
		break;
	}
}

bool Player::isJumping() {
	if(contactListener->getNumFootContacts() == 1) return false;
    else return true;
}

void Player::push(b2Vec2&& dir)
{
	box->getBody()->ApplyLinearImpulse(dir, box->getBody()->GetPosition(), true);
}

void Player::updatePlayer()
{
    frogSprite.setRotation((-box->getBody()->GetAngle() / 3.14) * 180);
	// Adjusting from sprite 0:0 to body 0.5:0.5
    frogSprite.setOrigin(25, 25);
//    float adjPlacementX = (frogSprite.getLocalBounds().right - frogSprite.getLocalBounds().left) / 2
    float adjPosX = (box->getBody()->GetPosition().x + 800 / 30.0 / 2) * 30.0;
    float adjPosY = (-box->getBody()->GetPosition().y + 600 / 30.0 / 2) * 30.0;
    
    frogSprite.setPosition(adjPosX, adjPosY); // 56824 85.230.64.105
//    std::cout << "Number of foot contacts: " << contactListener->getNumFootContacts() << std::endl;
	
	b2Vec2 oldSpeed = box->getBody()->GetLinearVelocity();
    oldSpeed = b2Vec2(0, oldSpeed.y);
	box->update();
    box->getBody()->SetLinearVelocity(b2Vec2(leftSpeed + rightSpeed, 0) + oldSpeed);
}