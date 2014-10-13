#include "Player.h"
#include "Game.h"
#include "OSHandler.h"
#include "Hook.h"
#include "Client.h"
#include "Server.h"
#include "PacketParser.h"

Player::Player(Game* game_) {
    game = game_;
	Player::name = name;
    // World, Size, Position, Density, Friction, Dynamic, Collision group
    box = new Rectangle(game_,
                        new b2Vec2(50.0f, 50.0f),
                        new b2Vec2(0, 0),
                        true,
						-1,
                        1.0,
                        0.1);
    box->getBody()->SetFixedRotation(true);
    box->getBody()->SetGravityScale(3);
    
	jumpHeight = 50;
    movementSpeed = 10;
    
    if (!frogTexture.loadFromFile(game->getOSHandler()->getResourcePath() + "frog_placeholder.png")) {
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
    myFixtureDef.density = 1;
    
    // ##### SENSOR #####
    // width, height, center, angle
    polygonShape.SetAsBox(0.3, 0.3, b2Vec2(0,-2), 0);
    myFixtureDef.isSensor = true;
    footSensorFixture = box->getBody()->CreateFixture(&myFixtureDef);
    
    // This is needed for the ContactListener to recognize footSensor(see ContactListener.cpp)
    footSensorFixture->SetUserData( (void*)1 );
    birthNumber = 1;
    
	// ##### HOOKSHOT #####
    // game object, radius, position, dynamic, density, friction
    // I need to send box here because game->getPlayer() will give BAD_ACCESS(not created)
    
    /*contactListener = new ContactListener();
    game->getWorld()->SetContactListener(contactListener);*/
    contactListener = game->getContactListener();

	leftSpeed = 0;
	rightSpeed = 0;
    hook = NULL;
	dead = false;
}

void Player::init(Player* player) {
	hook = new Hook(game, player);
}

b2Body* Player::getBody() {
    return box->getBody();
}

void Player::setName(string n)
{
	name = n;
}

void Player::setBirthNumber(int number) {
    birthNumber = number;
    footSensorFixture->SetUserData((void*)(uintptr_t)number);
	// also set hook number+10
}

void Player::setPosition(b2Vec2* newPos) {
	box->getBody()->SetTransform(*newPos, box->getBody()->GetAngle());
	b2Vec2 pos = box->getBody()->GetPosition();
	hook->getHookBase()->setPosition(&pos);
	pos.y += 5;
	hook->getHookTip()->setPosition(&pos);
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
            if(!isJumping() || (!localPlayer && is_jumping))
			{
				push(b2Vec2(0, jumpHeight));
				jumped = true;
			}
		break;
	default:
		break;
	}
	if(localPlayer)
	{
		//if client then tell server about movement
        sf::Packet packet;
		player_info p;
		p.name = name;
		p.movedir = dir;
		p.jumped = jumped;
		p.velocity = box->getBody()->GetLinearVelocity();
		packet = game->getPacketParser()->pack<player_info*>(UDPNetwork::PLAYER_MOVE, &p);
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

void Player::push(b2Vec2&& dir) {
	box->getBody()->ApplyLinearImpulse(dir, box->getBody()->GetPosition(), true);
}

void Player::useHook(sf::Vector2i mousePos) {
    if(hook != NULL && hook->getLength() < 0.7) hook->shoot(mousePos);
}

void Player::aimHook(sf::Vector2i mousePos) {
    if(hook != NULL) hook->aim(mousePos);
}

void Player::releaseHook() {
    if(hook != NULL) hook->release();
}

bool Player::isJumping() {
	if(contactListener->getNumFootContacts(birthNumber) > 0) return false;
    else return true;
}

void Player::draw() {
    game->getWindow()->draw(frogSprite);
    if(hook != NULL) hook->draw();
    
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
    box->getBody()->SetLinearVelocity(b2Vec2(leftSpeed + rightSpeed, 0) + oldSpeed);
    
    if(hook != NULL) hook->update();
    
//    cout << "player position: " << adjPosX << " " << adjPosY << endl;
}

