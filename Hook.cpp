#include "Hook.h"
#include "Circle.h"
#include "Game.h"
#include "Utility.h"
#include "ContactListener.h"
#include <cmath>
#include <math.h>
#include <iomanip>

Hook::Hook(Game* game_, Player* player) {
    cout << "Hook()" << endl;
    game = game_;
	owner = player;
    utility = game->getUtility();
    //contactListener = game->getPlayer()->getContactListener();
	contactListener = player->getContactListener();
    recentBoxContact = contactListener->getRecentHookContact(player->getBirthNumber());
    
    ACTION = PASSIVE;
    
    /*playerMeterPos = b2Vec2(game->getPlayer()->getBody()->GetPosition().x,
                            game->getPlayer()->getBody()->GetPosition().y);*/
	playerMeterPos = b2Vec2(player->getBody()->GetPosition().x,
                            player->getBody()->GetPosition().y);
    
    hookTip = new Circle(game,
                         5.0f,
                         new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                    (playerMeterPos.y + 5) * game->getUtility()->getMTP()),
                         true,
						 -1,
                         0.01,
                         0.0);
    
    // This way it will be recognized in the ContactListnener(see if statements ContactListener.cpp)
    hookTip->getBody()->GetFixtureList()->SetUserData( (void*)(uintptr_t)(player->getBirthNumber()+10));
    cout << "birthNumber: " << player->getBirthNumber() << endl;
    
    hookBase = new Rectangle(game,
                             new b2Vec2(10.0, 10.0),
                             new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                        (playerMeterPos.y) * game->getUtility()->getMTP()),
                             true,
							 -1,
                             1.0,
                             0.3);
    
    //EFTER ATT JAG IMPLEMENTERADE DETTA BUGGAR DET IBLAND
//    b2Filter filter = hookTip->getBody()->GetFixtureList()->GetFilterData();
//    filter.groupIndex = 0;
//    hookTip->getBody()->GetFixtureList()->SetFilterData(filter);
//    
//    cout << "player filter: " << game->getPlayer()->getBody()->GetFixtureList()->GetFilterData().groupIndex << endl;
//    
	hookTip->getShape()->setFillColor(sf::Color(255, 0, 0));
    hookBase->getShape()->setFillColor(sf::Color(0, 0, 255));
    
//    b2DistanceJointDef def;
//    def.bodyA = game->getBody()->getBody();
//    def.bodyB = hookTip->getBody();
//    def.localAnchorA = b2Vec2(0, 0);
//    def.localAnchorB = b2Vec2(0, 0);
//    def.length = 0;
//    def.collideConnected = false;
//    
//	distanceJoint = (b2DistanceJoint*)(game->getWorld()->CreateJoint(&def));
    
    passiveLength = 0.5;
    grabLength = 2.0;
    reachLength = 5.0;
    currentLength = passiveLength;
    
    //game->getBody()->getBody();
    b2RevoluteJointDef revDef;
    revDef.bodyA = hookBase->getBody();
    //revDef.bodyB = game->getPlayer()->getBody();
	revDef.bodyB = player->getBody();
//    revDef.collideConnected = false;
    revDef.localAnchorA = b2Vec2(0, 0);
    revDef.localAnchorB = b2Vec2(0, 0);
    revDef.enableLimit = true;
    revDef.enableMotor = true;
    revDef.lowerAngle = 0.0;
    revDef.upperAngle = 1.0;
    //    revDef.motorSpeed = 1;
    revDef.maxMotorTorque = 1000;
    
    revoluteJoint = (b2RevoluteJoint*)(game->getWorld()->CreateJoint(&revDef));
    
    
    b2PrismaticJointDef def;
    
    b2Vec2 axis = hookBase->getBody()->GetPosition() - hookTip->getBody()->GetPosition();
    axis.Normalize();
    
//    def.Initialize(hookBase->getBody(), hookTip->getBody(), b2Vec2(0, 0), axis);
    def.bodyA = hookBase->getBody();
    def.bodyB = hookTip->getBody();
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
//    def.collideConnected = false;
    def.enableLimit = true;
    def.enableMotor = true;
    def.lowerTranslation = 0.0f;
    def.upperTranslation = passiveLength;
    def.maxMotorForce = 10.0f;
    def.motorSpeed = 30.0f;
    
	prismaticJoint = (b2PrismaticJoint*)(game->getWorld()->CreateJoint(&def));
    
    grabJoint = NULL;
    
    newMouseAngle = 10;
}

void Hook::aim(sf::Vector2i mousePixelPos) {
    hookDegrees = utility->angleBetweenPoints(hookTip->getBody()->GetPosition(),
                                              hookBase->getBody()->GetPosition());
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos, hookDegrees);
	revoluteJoint->SetLimits(utility->degToRad(newMouseAngle), utility->degToRad(newMouseAngle));
    
//    cout << "hookDegrees: " << hookDegrees << "    mouseAngle: " << newMouseAngle << endl;
}

void Hook::shoot(sf::Vector2i mousePixelPos) {
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos, hookDegrees);
    prismaticJoint->SetLimits(0.0, reachLength);

    // We want too look for objects to grab
    contactListener->setHookActive(true);
    ACTION = SHOOTING;
}

b2RevoluteJoint* Hook::grab(b2Body* box) {
    // Makes the box more easily handled by frog
    box->GetFixtureList()->SetDensity(0.0001);
//    box->GetFixtureList()->SetFriction(0.1);
    box->SetFixedRotation(true);
    box->ResetMassData();
    
    b2RevoluteJointDef def;
    def.bodyA = hookTip->getBody();
    def.bodyB = box;
    def.collideConnected = true;
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
    
    return (b2RevoluteJoint*)game->getWorld()->CreateJoint(&def);
}

void Hook::release() {
    if(grabJoint != NULL) {
        prismaticJoint->SetLimits(0.0, passiveLength);
        // Return to original density and friction
        grabJoint->GetBodyB()->GetFixtureList()->SetDensity(1.0);
        grabJoint->GetBodyB()->GetFixtureList()->SetFriction(1.0);
        grabJoint->GetBodyB()->SetFixedRotation(false);
        grabJoint->GetBodyB()->SetLinearVelocity(b2Vec2(0, 0));
        grabJoint->GetBodyB()->ResetMassData();
        
        // Destroy the joint connecting hook and box
    	game->getWorld()->DestroyJoint(grabJoint);
    	grabJoint = NULL;
        
        // Remove the recent contact, we no longer want to grab it
        contactListener->removeRecentHookContact(game->getPlayer()->getBirthNumber());
        
        // Make it so that we dont grab boxes while hook is returning to us
    	contactListener->setHookActive(false);
        ACTION = PASSIVE;
    }
}

void Hook::update() {
    hookTip->update();
    hookBase->update();
    /*playerMeterPos = b2Vec2(game->getPlayer()->getBody()->GetPosition().x,
                            game->getPlayer()->getBody()->GetPosition().y);*/

	playerMeterPos = b2Vec2(owner->getBody()->GetPosition().x,
                            owner->getBody()->GetPosition().y);
	;
	/*if(recentBoxContact != NULL && ((b2BodyUserData*)recentBoxContact->GetUserData())->toBeRemoved)
		release();*/
    
    switch(ACTION)
    {
        case SHOOTING:
            recentBoxContact = contactListener->getRecentHookContact(game->getPlayer()->getBirthNumber());
            
			if(recentBoxContact != NULL && grabJoint == NULL) {
                prismaticJoint->SetLimits(0.0, grabLength);
                grabJoint = grab(recentBoxContact);
                ACTION = PASSIVE;
            }
            else if(recentBoxContact == NULL && getLength() > (reachLength - 0.2)) {
                prismaticJoint->SetLimits(0.0, passiveLength);
                ACTION = PASSIVE;
            }
            break;
        case PASSIVE:
            break;
            
        default:
            cout << "Invalid action" << endl;
    }
//    
//    if(recentBoxContact == NULL && getLength() > (reachLength - 0.2)) {
//        cout << "OVER " << (reachLength - 0.2) << endl;
//        prismaticJoint->SetLimits(0.0, passiveLength);
//    }
//	
//    if(recentBoxContact != NULL && weldJoint == NULL) {
//        prismaticJoint->SetLimits(0.0, grabLength);
//        
//        weldJoint = grab(recentBoxContact);
//    }
}

void Hook::draw() {
    game->getWindow()->draw(*hookTip->getShape());
	game->getWindow()->draw(*hookBase->getShape());
}

float Hook::getLength() {
    b2Vec2 A = b2Vec2(hookBase->getBody()->GetPosition().x,
                      hookBase->getBody()->GetPosition().y);
    b2Vec2 B = b2Vec2(hookTip->getBody()->GetPosition().x,
                      hookTip->getBody()->GetPosition().y);
    
    float length = sqrt(pow(std::abs(A.x - B.x), 2) + pow(std::abs(A.y - B.y), 2));
    return length;
}