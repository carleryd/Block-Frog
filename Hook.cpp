//
//  Hook.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-29.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Hook.h"
#include "Circle.h"
#include "Game.h"
#include <cmath>
#include <math.h>


Hook::Hook(Game* game_) {
    game = game_;
    float playerX = game->getPlayer()->getBody()->GetPosition().x;
    float playerY = game->getPlayer()->getBody()->GetPosition().y;
    cout << playerX << " " << playerY << endl;
    hookTip = new Circle(game, 5.0f, new b2Vec2(playerX * game->getMeterToPixel(), playerY * game->getMeterToPixel()), true, 0.0, 0.0);
	hookTip->getShape()->setFillColor(sf::Color(255, 0, 0));
    hookTip->getBody()->ApplyLinearImpulse(b2Vec2(-1, 1), hookTip->getBody()->GetPosition(), true);
    
    b2DistanceJointDef* def = new b2DistanceJointDef();
    // bodyA, bodyB, localAnchorA, localAnchorB
    def->Initialize(game->getPlayer()->getBody(), hookTip->getBody(), b2Vec2(0, 0), b2Vec2(0, 0));
    def->length = 3;
    def->collideConnected = false;
	distanceJoint = dynamic_cast<b2DistanceJoint*>(game->getWorld()->CreateJoint(def));
}

void Hook::draw() {
    game->getWindow()->draw(*hookTip->getShape());
}

void Hook::update() {
    hookTip->update();
}

float Hook::getLength() {
    b2Vec2* A = new b2Vec2(game->getPlayer()->getBody()->GetPosition().x, game->getPlayer()->getBody()->GetPosition().y);
    b2Vec2* B = new b2Vec2(hookTip->getBody()->GetPosition().x, hookTip->getBody()->GetPosition().y);
//	cout << "A: " << A->x << " " << A->y << endl;
//  	cout << "B: " << B->x << " " << B->y << endl;
    float length = sqrt(pow(std::abs(A->x - B->x), 2) + pow(std::abs(A->y - B->y), 2));
    return length;
}

void Hook::changeLength(float delta) {
    distanceJoint->SetLength(distanceJoint->GetLength() + delta);
}