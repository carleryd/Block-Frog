#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <SFML/Network.hpp>
#include "ShapeFactory.h"
class Shape;

class PacketParser
{
public:
	PacketParser(ShapeFactory& factory);
	PacketParser(); //cannot unpack shapes
	~PacketParser(void);
	sf::Packet packageShape(Shape* shape);
	/*
		This function will ONLY unpack ONE shape from the packet!
	*/
	Shape* unpackageShape(sf::Packet& p); 
private:
	ShapeFactory& factory;
};

#endif