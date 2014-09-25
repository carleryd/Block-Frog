#include "PacketParser.h"
#include "Shape.h"
#include "ShapeFactory.h"
#include "UDPNetwork.h"
#include <iostream>

PacketParser::PacketParser(ShapeFactory& f):
	factory(f)
{
}


PacketParser::~PacketParser(void)
{
}

sf::Packet PacketParser::packageShape(Shape* shape)
{
	sf::Packet packet;
	b2Body* body = shape->getBody();
	int type = UDPNetwork::SHAPE;
	packet << type; //type
	packet << body->GetPosition().x << body->GetPosition().y;
	packet << shape->getSize()->x << shape->getSize()->y;
	return packet;
}

Shape* PacketParser::unpackageShape(sf::Packet& packet)
{
	int type;
	b2Vec2 pos, size;
	if(!(packet >> pos.x >> pos.y >> size.x >> size.y))
		std::cerr << "Error occured in received data!" << endl;
	return factory.createRectangle(&size, &pos, true);
}