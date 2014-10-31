#include "Player.h"
#include "Game.h"
#include "OSHandler.h"
#include "Hook.h"
#include "Client.h"
#include "Server.h"
#include "PacketParser.h"
#include "Item.h"
#include "Utility.h"

Player::Player(Game* game_, int localID_, string color) {
    cout << "color: " << color << endl;
    game = game_;
    localID = localID_;
    alias = "noname";
    window = game->getWindow();
    // World, Size, Position, Dynamic, Density, Friction, Collision group
    box = new Rectangle(game_,
                        new b2Vec2(50.0f, 50.0f),
                        new b2Vec2((localID-4) * (int)window->getSize().x/8, -200),
                        true,
						-1,
                        0.1,
                        1.0,
                        3);
    box->getBody()->SetFixedRotation(true);
    
    // Jump parameters
	jumpHeight = 15;
    downWardPull = -0.6;
    linearDamping = 1;
    // Left and right movement
    movementSpeed = 15;
    
    if (!frogTexture.loadFromFile(game->getOSHandler()->getResourcePath() + "frog_" + color + ".png")) {
        std::cout << "Could not load frog image" << std::endl;
    }
    
    frogSprite.setTexture(frogTexture);
    frogSprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
    frogSprite.setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
    
    // Foot sensor - this is a small shape without physics
    // it is only used as a sensor, placed underneath the player so
    // that we can disable the jumping capabilities while in air
    b2PolygonShape polygonShape;
    
    b2FixtureDef myFixtureDef;
    myFixtureDef.shape = &polygonShape;
    myFixtureDef.density = 0.0001;
    
    // ##### SENSOR #####
    // width, height, center, angle
    // O.98 will stop player form being able to jump riiight next to a block without touching ground
    float playerMeterWidth = box->getSize()->x/2 * game->getUtility()->getPTM();
    float playerMeterHeight = box->getSize()->y/2 * game->getUtility()->getPTM();
    polygonShape.SetAsBox(0.98 * playerMeterWidth,
                          0.05, b2Vec2(0, -playerMeterHeight), 0);
    myFixtureDef.isSensor = true;
    footSensorFixture = box->getBody()->CreateFixture(&myFixtureDef);
    
    // This is needed for the ContactListener to recognize footSensor(see ContactListener.cpp)
    footSensorFixture->SetUserData((void*)(uintptr_t)localID);
    
	// ##### HOOKSHOT #####
    // game object, radius, position, dynamic, density, friction
    // I need to send box here because game->getPlayer() will give BAD_ACCESS(not created)
    
    /*contactListener = new ContactListener();
    game->getWorld()->SetContactListener(contactListener);*/
    contactListener = game->getContactListener();

	leftSpeed = 0;
	rightSpeed = 0;
    hook = nullptr;
	dead = false;
}

Player::~Player() {
	destroyHook();
    
    delete box;
    box = nullptr;
    footSensorFixture = nullptr;

    game = nullptr;
}

void Player::destroyHook() {
    delete hook;
    hook = nullptr;
}

void Player::init() {
	hook = new Hook(game, this);
}

Hook* Player::getHook() {
    if(hook == nullptr)
        hook = new Hook(game, this);
	return hook;
}

int Player::getLocalID() {
	return localID;
}

void Player::resetPlayer(b2Vec2* newPos) {
    b2Vec2 hookTipPos = box->getBody()->GetPosition() - *hook->getHookTip()->getPosition();
    box->getBody()->SetTransform(*newPos, box->getBody()->GetAngle());
	b2Vec2 pos = box->getBody()->GetPosition();
	if(hook != nullptr)
	{
        hook->release();
		hook->getHookBase()->setPosition(&pos);
		pos += hookTipPos;
		hook->getHookTip()->setPosition(&pos);
	}
}

void Player::setPosition(b2Vec2* newPos) {
	box->getBody()->SetTransform(*newPos, box->getBody()->GetAngle());
}

void Player::move(int dir, bool localPlayer, bool is_jumping)
{
	bool jumped = false;
	switch (dir)
	{
	case LEFT:
            leftSpeed = -movementSpeed;
		break;
	case RIGHT:
			rightSpeed = movementSpeed;
		break;
    case LEFT_STOP:
            leftSpeed = 0;
        break;
    case RIGHT_STOP:
            rightSpeed = 0;
        break;
	case JUMP:
            if(!isJumping())// || (!localPlayer && is_jumping))
			{
				push(b2Vec2(0, jumpHeight));
				jumped = true;
			}
		break;
	default:
		break;
	}
	if(game->getLocalHost() != NULL && localPlayer)
	{
		//if client then tell server about movement
        sf::Packet packet;
		player_info p;
		p.localID = localID;
		p.movedir = dir;
		p.jumped = jumped;
		p.velocity = box->getBody()->GetLinearVelocity();
		packet = game->getPacketParser()->pack<player_info*>(UDPNetwork::PLAYER_MOVE, &p);
		send(packet);
		/*if(game->getLocalHost()->isServer())
		{
			Server* server = dynamic_cast<Server*>(game->getLocalHost());
			server->broadCast(packet);
		}
		else
		{
			dynamic_cast<Client*>(game->getLocalHost())->sendToServer(packet);
		}*/
	}
}

void Player::push(b2Vec2&& dir) {
	box->getBody()->ApplyLinearImpulse(dir, box->getBody()->GetPosition(), true);
}

void Player::useHook(sf::Vector2i mousePos, bool local) {
    if(hook != nullptr && hook->getLength() < hook->getPassiveLength() + 0.1)
	{
		hook->shoot(mousePos);
		if(local)
		{
			hook_info h;
			h.localID = localID;
			h.mousePos = mousePos;
			sf::Packet p = game->getPacketParser()->pack(UDPNetwork::HOOK_SHOT, h);
			send(p);
		}
	}
}

void Player::aimHook(sf::Vector2i mousePos, bool local) {
    if(hook != nullptr)
	{
		hook->aim(mousePos);
		if(local)
		{
			hook_info h;
			h.localID = localID;
			h.mousePos = mousePos;
			sf::Packet p = game->getPacketParser()->pack(UDPNetwork::HOOK_AIM, h);
			send(p);
		}
	}
}

void Player::releaseHook(bool local) {
    if(hook != nullptr)
	{
		hook->release();
		if(local)
		{
			sf::Packet p = game->getPacketParser()->pack<int>(UDPNetwork::HOOK_RELEASE, localID);
			send(p);
		}

	}
}

bool Player::isJumping() {
	if(contactListener->getNumFootContacts(localID) > 0) return false;
    else return true;
}

void Player::draw() {
    window->draw(frogSprite);
    if(hook != nullptr) hook->draw();
    
    frogSprite.setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
}

void Player::update() {
    frogSprite.setRotation( float( (-box->getBody()->GetAngle() / 3.14) * 180));
	// Adjusting from sprite 0:0 to body 0.5:0.5
    frogSprite.setOrigin(25, 25);
    float adjPosX = (box->getBody()->GetPosition().x + game->getWindow()->getSize().x / 30.0 / 2) * 30.0;
    float adjPosY = (-box->getBody()->GetPosition().y + game->getWindow()->getSize().y / 30.0 / 2) * 30.0;
    
    frogSprite.setPosition(adjPosX, adjPosY);
	
	b2Vec2 oldSpeed = box->getBody()->GetLinearVelocity();
    oldSpeed = b2Vec2(0, oldSpeed.y);
	box->update();
//    if(leftSpeed + rightSpeed != 0)

    box->getBody()->SetLinearVelocity(b2Vec2(leftSpeed + rightSpeed, 0) + oldSpeed);
    // Artificially increase player gravity
    box->getBody()->ApplyLinearImpulse(b2Vec2(0, downWardPull), box->getBody()->GetPosition(), true);
    box->getBody()->SetLinearDamping(linearDamping);
    
    if(hook != nullptr)
		hook->update();

	itemCheck();
    
//    cout << "player position: " << adjPosX << " " << adjPosY << endl;
}

void Player::itemCheck()
{
	b2Body* body = box->getBody();
	b2ContactEdge* edge = body->GetContactList();
	Item* touchedItem = nullptr;
	for(;edge; edge = edge->next)
	{
		if(edge->contact->IsTouching() &&
			((b2BodyUserData*)edge->other->GetUserData())->isItem)
		{
			touchedItem = ((Item*)((b2BodyUserData*)edge->other->GetUserData())->parent);
			touchedItem->causeEffect(this);
		}
	}
	if(touchedItem != nullptr)
		game->removeShape(touchedItem->getId());
}

void Player::send(sf::Packet& packet)
{ 
	if(game->getLocalHost() != NULL)
	{
		if(game->getLocalHost()->isServer())
		{
			Server* server = dynamic_cast<Server*>(game->getLocalHost());
			server->broadCast(packet);
		}
		else
		{
			dynamic_cast<Client*>(game->getLocalHost())->sendToServer(packet);
		}
	}
}