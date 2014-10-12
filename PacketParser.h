#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <SFML/Network.hpp>
#include "ShapeFactory.h"
#include "Player.h"
class Shape;
class Player;

struct shapeSync
{
	shapeSync(){};
	shapeSync(Shape& shape)
	{
		shapeID = shape.getId();
		angularVel = shape.getBody()->GetAngularVelocity();
		velocity = shape.getBody()->GetLinearVelocity();
		position = shape.getBody()->GetPosition();
		angle = shape.getBody()->GetAngle();
		hookUserData = (uintptr_t)shape.getBody()->GetFixtureList()->GetUserData();
	};
	int shapeID;
	float angularVel;
	b2Vec2 velocity;
	b2Vec2 position;
	float angle;
	int hookUserData;
};

struct player_info
{
	player_info(){};
	player_info(Player& p)
	{
		name = p.getName();
		movedir = -1; // illegal is set in player::move
		jumped = false;  // illegal is set in player::move
		velocity = p.getBody()->GetLinearVelocity();
		position = p.getBody()->GetPosition();
		hookTip = shapeSync(*p.getHookTip());
		hookBase = shapeSync(*p.getHookBase());
	}
	string name;
	int movedir;
	bool jumped; 
	b2Vec2 velocity;
	b2Vec2 position;
	shapeSync hookTip;
	shapeSync hookBase;
};

class PacketParser
{
public:
	PacketParser(ShapeFactory& factory);
	PacketParser(); //cannot unpack shapes
	~PacketParser(void);
	sf::Packet pack(Shape* shape);
	/*
		for when the player acts(moves)
		- use PLAYER_MOVE for only communicating movement
			+ position not necessary for this type
		- user PLAYER_SYNCH for synchronize all of the player
			+ movedir and jumped not necessary for this type
	*/
	//USE pack<T>(int, T);
	/*template<class T>
		sf::Packet pack(int type, T player_info);*/

	//for when sending the NEW_PLAYER type packet
	sf::Packet pack(Player*);
	/*
		pack necesary data to sync shapes
		appendee determines if the packet is to be appended to another packet or not
		appendee = false sets a type to the packet so that it can be handled by the receiver
	*/
	sf::Packet pack(shapeSync* s, bool appendee = false);

	//for packing primitive types
	template<typename T>
	sf::Packet pack(int type, T primitiveType);
	
	


	//template function SHOULD be able to unpack ONE of any object
	//define template for new T if it does not exist, as they must be explicitly defined
	template<typename T>
	T unpack(sf::Packet& packet);

private:
	ShapeFactory& factory;
};

/*sf::Packet& operator<<(sf::Packet& appendto, const sf::Packet appendwith)
{
	appendto.append(appendwith.getData(), appendwith.getDataSize());
	return appendto;
};*/

#endif