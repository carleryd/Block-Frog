#include "Hook.h"
#include "Circle.h"
#include "Game.h"
#include "Utility.h"
#include "ContactListener.h"
#include <cmath>
#include <math.h>
#include <iomanip>


Hook::Hook(Game* game_) {
    cout << "Hook()" << endl;
    game = game_;
    utility = game->getUtility();
    contactListener = game->getPlayer()->getContactListener();
    
    hookTip = new Circle(game,
                         5.0f,
                         new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                    (playerMeterPos.y + 5) * game->getUtility()->getMTP()),
                         true,
                         0.1,
                         0.2,
                         -1);
    // This way it will be recognized in the ContactListnener(see if statements ContactListener.cpp)
    hookTip->getBody()->GetFixtureList()->SetUserData( (void*)4 );
    
    hookBase = new Rectangle(game,
                             new b2Vec2(10.0, 10.0),
                             new b2Vec2(playerMeterPos.x * game->getUtility()->getMTP(),
                                        playerMeterPos.y * game->getUtility()->getMTP()),
                             true,
                             5.0,
                             0.3,
                             -1);
    
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
    
    passiveLength = 1.0;
    useLength = 8.0;
    
    //game->getBody()->getBody();
    b2RevoluteJointDef revDef;
    revDef.bodyA = hookBase->getBody();
    revDef.bodyB = game->getPlayer()->getBody();
    revDef.collideConnected = false;
    revDef.localAnchorA = b2Vec2(0, 0);
    revDef.localAnchorB = b2Vec2(0, 0);
    revDef.enableLimit = true;
    revDef.enableMotor = true;
    //    revDef.motorSpeed = 1;
    revDef.maxMotorTorque = 1000;
    
    
    revoluteJoint = (b2RevoluteJoint*)(game->getWorld()->CreateJoint(&revDef));
    
    b2PrismaticJointDef def;
    def.bodyA = hookBase->getBody();
    def.bodyB = hookTip->getBody();
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
    def.lowerTranslation = 0.0;
    def.upperTranslation = 0.5;
    def.enableLimit = true;
    def.maxMotorForce = 100000;
    def.motorSpeed = 100;
    def.enableMotor = true;
    def.collideConnected = false;
    
	prismaticJoint = (b2PrismaticJoint*)(game->getWorld()->CreateJoint(&def));
    
    weldJoint = NULL;
    
    oldMouseAngle = 999;
    newMouseAngle = 999;
}

void Hook::aim(sf::Vector2i mousePixelPos) {
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos);
    
    oldMouseAngle = newMouseAngle;
    
	revoluteJoint->SetLimits(utility->degToRad(newMouseAngle), utility->degToRad(newMouseAngle));
}

void Hook::use(sf::Vector2i mousePixelPos) {
    newMouseAngle = utility->mouseAngle(mousePixelPos, playerMeterPos);
    
    prismaticJoint->SetLimits(0.0, 8.0);
}

void Hook::release() {
    if(weldJoint != NULL) {
//        weldJoint->GetBodyB()->GetFixtureList()->SetDensity(1.0);
//        weldJoint->GetBodyB()->GetFixtureList()->SetFriction(1.0);
//        weldJoint->GetBodyB()->ResetMassData();
        // krashar när jag lade till det här, fixa det!
        
    	game->getWorld()->DestroyJoint(weldJoint);
    	weldJoint = NULL;
    }
}

void Hook::draw() {
    game->getWindow()->draw(*hookTip->getShape());
	game->getWindow()->draw(*hookBase->getShape());
}

void Hook::update() {
    hookTip->update();
    hookBase->update();
    playerMeterPos = b2Vec2(game->getPlayer()->getBody()->GetPosition().x,
                            game->getPlayer()->getBody()->GetPosition().y);
//    cout << "hookBase angle: " << hookBase->getBody()->GetAngle() << endl;
    if(getLength() > 7.8 || contactListener->getRecentHookContact() != NULL) {
        prismaticJoint->SetLimits(0.0, 0.5);
    }
	
    b2Body* recentBoxContact = contactListener->getRecentHookContact();
    if(recentBoxContact != NULL && weldJoint == NULL) {
	    cout << "hit box X: " << recentBoxContact->GetPosition().x
        	<< " Y: " << recentBoxContact->GetPosition().y << endl;
        contactListener->removeRecentHookContact();
        weldJoint = grab(recentBoxContact);
    }
}

b2WeldJoint* Hook::grab(b2Body* box) {
//    box->GetFixtureList()->SetDensity(0.1);
//    box->GetFixtureList()->SetFriction(0.1);
//    box->ResetMassData();
    
    b2WeldJointDef def;
    def.bodyA = hookTip->getBody();
    def.bodyB = box;
    def.collideConnected = true;
    def.localAnchorA = b2Vec2(0, 0);
    def.localAnchorB = b2Vec2(0, 0);
    
    return (b2WeldJoint*)game->getWorld()->CreateJoint(&def);
}

float Hook::getLength() {
    b2Vec2 A = b2Vec2(hookBase->getBody()->GetPosition().x,
                      hookBase->getBody()->GetPosition().y);
    b2Vec2 B = b2Vec2(hookTip->getBody()->GetPosition().x,
                      hookTip->getBody()->GetPosition().y);
    
    float length = sqrt(pow(std::abs(A.x - B.x), 2) + pow(std::abs(A.y - B.y), 2));
    return length;
}