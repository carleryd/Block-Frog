#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <SFML/Network.hpp>
#include "ShapeFactory.h"
class Shape;
class Player;

struct player_info
{
	string name;
	int movedir;
	bool jumped; 
	b2Vec2 velocity;
};

struct shapeSync
{
	int shapeID;
	float angularVel;
	b2Vec2 velocity;
	b2Vec2 position;
	float angle;
	b2Vec2 size;
};

class PacketParser
{
public:
	PacketParser(ShapeFactory& factory);
	PacketParser(); //cannot unpack shapes
	~PacketParser(void);
	sf::Packet pack(Shape* shape);
	//for when the player acts(moves)
	sf::Packet pack(player_info p);
	//for when sending the NEW_PLAYER type packet
	sf::Packet pack(Player*);
	//pack necesary data to sync shapes
	sf::Packet pack(shapeSync* s);

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