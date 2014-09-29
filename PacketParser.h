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
};

class PacketParser
{
public:
	PacketParser(ShapeFactory& factory);
	PacketParser(); //cannot unpack shapes
	~PacketParser(void);
	sf::Packet pack(Shape* shape);
	sf::Packet pack(player_info p);

	/*template<typename primitiveType>
	sf::Packet pack(int type, primitiveType t)
	{
		sf::Packet packet;
		packet << type;
		packet << t;
		return packet;
	};*/
	//sf::Packet pack(b2Vec2* vector);
	/*template<typename T>
	sf::Packet pack(T* object);*/
	/*
		This function will ONLY unpack ONE shape from the packet!
	*/
	Shape* unpackageShape(sf::Packet& p); 
	b2Vec2* unpackB2Vector(sf::Packet& p);

	//template function SHOULD be able to back ONE of any object
	template<typename T>
		T* unpack(sf::Packet& packet);

private:
	ShapeFactory& factory;
};

#endif