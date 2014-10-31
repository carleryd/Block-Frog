#include "Hook.h"
#include "Player.h"
#include "Circle.h"
#include "Game.h"
#include "Utility.h"
#include "ContactListener.h"
#include <cmath>
#include <math.h>
#include <iomanip>

Hook::Hook(Game* game_, Player* player_) {
    game = game_;
    // We need to pass player to avoid BAD_ACCESS
	player = player_;
    localID = player->getLocalID();
    cout << "Player localID: " << localID << endl;
    utility = game->getUtility();
	contactListener = game->getContactListener();
    recentBoxContact = contactListener->getRecentHookContact(localID);
    
    
    ACTION = PASSIVE;
    
	playerMeterPos = b2Vec2(player->getBody()->GetPosition().x,
                            player->getBody()->GetPosition().y);
    
    hookTip = new Circle(game,
                         5.0f,
                         new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                    (playerMeterPos.y + 5) * game->getUtility()->getMTP()),
                         true,
						 -1,
                         0.01,
                         0.0,
                         3);
    
    // This way it will be recognized in the ContactListnener(see if statements ContactListener.cpp)
    hookTip->getBody()->GetFixtureList()->SetUserData((void*)(uintptr_t)(localID+10));
    
    hookBase = new Rectangle(game,
                             new b2Vec2(10.0, 10.0),
                             new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                        (playerMeterPos.y) * game->getUtility()->getMTP()),
                             true,
							 -1,
                             1.0,
                             0.3,
                             3);
    
	hookTip->getShape()->setFillColor(sf::Color(255, 0, 0));
    hookBase->getShape()->setFillColor(sf::Color(0, 0, 255));
    
    passiveLength = 0.6;
    grabLength = 2.0;
    reachLength = 5.0;
    currentLength = passiveLength;
    minLength = 0.0;
    
    b2RevoluteJointDef revDef;
    revDef.bodyA = hookBase->getBody();
	revDef.bodyB = player->getBody();
    revDef.localAnchorA = b2Vec2(0, 0);
    revDef.localAnchorB = b2Vec2(0, 0);
    revDef.enableLimit = true;
    revDef.enableMotor = true;
    revDef.lowerAngle = 0.0;
    revDef.upperAngle = 1.0;
    revDef.maxMotorTorque = 100;
    
    revoluteJoint = (b2RevoluteJoint*)(game->getWorld()->CreateJoint(&revDef));
    
    
    b2PrismaticJointDef def;
    
    b2Vec2 axis = hookBase->getBody()->GetPosition() - hookTip->getBody()->GetPosition();
    axis.Normalize();
    
    def.bodyA = hookBase->getBody();
    def.bodyB = hookTip->getBody();
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
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

Hook::~Hook() {
    player = nullptr;
    utility = nullptr;
    contactListener = nullptr;
    recentBoxContact = nullptr;
    
	game->getWorld()->DestroyJoint(revoluteJoint);
    revoluteJoint = nullptr;
    game->getWorld()->DestroyJoint(prismaticJoint);
    prismaticJoint = nullptr;
    
    delete hookTip;
    hookTip = nullptr;
    delete hookBase;
    hookBase = nullptr;
    
    game = nullptr;
}

void Hook::aim(sf::Vector2i mousePixelPos) {
    hookDegrees = 1.0;//utility->angleBetweenPoints(hookTip->getBody()->GetPosition(),
                    //                          hookBase->getBody()->GetPosition());
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos, hookDegrees);
	revoluteJoint->SetLimits(utility->degToRad(newMouseAngle), utility->degToRad(newMouseAngle));
}

void Hook::shoot(sf::Vector2i mousePixelPos) {
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos, hookDegrees);
    prismaticJoint->SetLimits(minLength, reachLength);

    // We want too look for objects to grab
    contactListener->setHookActive(true, localID);
    ACTION = SHOOTING;
}

b2RevoluteJoint* Hook::grab(b2Body* box) {
    grabbedBox = box;
    // Makes the box more easily handled by frog
    if(box->GetType() == b2BodyType::b2_dynamicBody) { // to be removed
	    box->GetFixtureList()->SetDensity(0.0001);
    	box->SetFixedRotation(true);
        box->ResetMassData();
    }
    
    b2RevoluteJointDef def;
    def.bodyA = hookTip->getBody();
    def.bodyB = box;
    def.collideConnected = true;
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
    
    return (b2RevoluteJoint*)game->getWorld()->CreateJoint(&def);
}

void Hook::release() {
    if(grabJoint != nullptr) {
        prismaticJoint->SetLimits(minLength, passiveLength);
        // Return to original density and friction
        if(grabJoint->GetBodyB()->GetType() == b2BodyType::b2_dynamicBody) {
	        grabJoint->GetBodyB()->GetFixtureList()->SetDensity(1.0);
    	    grabJoint->GetBodyB()->SetFixedRotation(false);
        	grabJoint->GetBodyB()->SetLinearVelocity(b2Vec2(0, 0));
            grabJoint->GetBodyB()->ResetMassData();
        }
        
        // Destroy the joint connecting hook and box
    	game->getWorld()->DestroyJoint(grabJoint);
    	grabJoint = nullptr;
        
        // Remove the recent contact, we no longer want to grab it
        contactListener->removeRecentHookContact(localID);
        
        // Make it so that we dont grab boxes while hook is returning to us
    	contactListener->setHookActive(false, localID);
        ACTION = PASSIVE;
    }
}

void Hook::update() {
    hookTip->update();
    hookBase->update();
	playerMeterPos = b2Vec2(player->getBody()->GetPosition().x,
                            player->getBody()->GetPosition().y);
    
//	cout << "--- HOOK POSITIONS ---" << endl;
//    cout << "hookTip: x: " << hookTip->getBody()->GetPosition().x << " y: " << hookTip->getBody()->GetPosition().y << endl;
//    cout << "hookBase: x: " << hookBase->getBody()->GetPosition().x << " y: " << hookBase->getBody()->GetPosition().y << endl;
	
	/*if(recentBoxContact != NULL && ((b2BodyUserData*)recentBoxContact->GetUserData())->toBeRemoved)
		release();*/
    
    switch(ACTION)
    {
        case SHOOTING:
            recentBoxContact = contactListener->getRecentHookContact(localID);
            
			if(recentBoxContact != NULL && grabJoint == NULL) {
                prismaticJoint->SetLimits(minLength, grabLength);
                grabJoint = grab(recentBoxContact);
                ACTION = PASSIVE;
            }
            else if(recentBoxContact == NULL && getLength() > (reachLength - 0.01)) {
                contactListener->setHookActive(false, localID);
                prismaticJoint->SetLimits(minLength, passiveLength);
                ACTION = PASSIVE;
            }
            break;
        case PASSIVE:
            break;
            
        default:
            cout << "Invalid action" << endl;
    }
}

void Hook::draw() {
	game->getWindow()->draw(*hookBase->getShape());
    game->getWindow()->draw(*hookTip->getShape());
}

float Hook::getLength() {
    b2Vec2 A = b2Vec2(hookBase->getBody()->GetPosition().x,
                      hookBase->getBody()->GetPosition().y);
    b2Vec2 B = b2Vec2(hookTip->getBody()->GetPosition().x,
                      hookTip->getBody()->GetPosition().y);
    
    float length = sqrt(pow(std::abs(A.x - B.x), 2) + pow(std::abs(A.y - B.y), 2));
    return length;
}