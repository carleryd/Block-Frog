#include "Player.h"
#include "Game.h"
#include "OSHandler.h"
#include "Client.h"
#include "Server.h"
#include "PacketParser.h"

Player::Player(Game* game_) {
    game = game_;
    
    // World, Size, Position, Density, Friction, FixedRotation
    box = new Rectangle(game, new b2Vec2(50.0f, 50.0f), new b2Vec2(0, 0), true);
    box->getBody()->SetFixedRotation(true);
    box->getBody()->SetGravityScale(3);
    
	jumpHeight = 50;
    
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
    
    // Sensor: width, height, center, angle
    polygonShape.SetAsBox(0.3, 0.3, b2Vec2(0,-2), 0);
    myFixtureDef.isSensor = true;
    b2Fixture* footSensorFixture = box->getBody()->CreateFixture(&myFixtureDef);
    
    // This is needed so that it can hold data about the "box" object
    footSensorFixture->SetUserData( (void*)3 );
    
	// Hookshot
    hookTip = new Rectangle(game, new b2Vec2(10.0f, 10.0f), new b2Vec2(200, 100), true);
    hookTip->getShape()->setFillColor(sf::Color(255, 0, 0));
    
    contactListener = new ContactListener();
    game->getWorld()->SetContactListener(contactListener);

	leftSpeed = 0;
	rightSpeed = 0;
}

void Player::draw() {
    game->getWindow()->draw(frogSprite);
    game->getWindow()->draw(*hookTip->getShape());
    
    frogSprite.setPosition(box->getBody()->GetPosition().x, box->getBody()->GetPosition().y);
}

void Player::setName(string n)
{
	name = n;
}

void Player::setPosition(b2Vec2* newPos) {
	box->getBody()->SetTransform(*newPos, box->getBody()->GetAngle());
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
			{
				push(b2Vec2(0, jumpHeight));
			}
		break;
	default:
		break;
	}
	//if client then tell server about movement
	player_info p;
	p.name = name;
	p.movedir = dir;
	if(game->getLocalHost()->isServer())
	{
		Server* server = dynamic_cast<Server*>(game->getLocalHost());
		/*server->broadCastExcept(server->getMyAddress(), server->getMyPort(),game->getPacketParser()->pack(p));*/
		//server->broadCast(game->getPacketParser()->pack(p));
		//dynamic_cast<Server*>(game->getLocalHost())->addPlayerInfo(new player_info(p));
	}
	else
	{
		dynamic_cast<Client*>(game->getLocalHost())->sendToServer(
			game->getPacketParser()->pack(p)
			);
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
    float adjPosX = (box->getBody()->GetPosition().x + game->getWindow()->getSize().x / 30.0 / 2) * 30.0;
    float adjPosY = (-box->getBody()->GetPosition().y + game->getWindow()->getSize().y / 30.0 / 2) * 30.0;
    
    frogSprite.setPosition(adjPosX, adjPosY); // 56824 85.230.64.105
	
	b2Vec2 oldSpeed = box->getBody()->GetLinearVelocity();
    oldSpeed = b2Vec2(0, oldSpeed.y);
	box->update();
    box->getBody()->SetLinearVelocity(b2Vec2(leftSpeed + rightSpeed, 0) + oldSpeed);
    hookTip->update();
}