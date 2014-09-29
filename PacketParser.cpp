#include "PacketParser.h"
//#include "Shape.h"
#include "ShapeFactory.h"
#include "UDPNetwork.h"
#include <iostream>
#include "Player.h"
#include "Rectangle.h"

PacketParser::PacketParser(ShapeFactory& f):
	factory(f)
{
}


PacketParser::~PacketParser(void)
{
}

sf::Packet PacketParser::pack(Shape* shape)
{
    Rectangle* rectangle = dynamic_cast<Rectangle*>(shape);
    sf::Packet packet;
    
    if(rectangle != nullptr) {
        b2Body* body = rectangle->getBody();
        int type = UDPNetwork::SHAPE;
        packet << type; //type
        packet << body->GetPosition().x << body->GetPosition().y;
        packet << rectangle->getSize()->x << rectangle->getSize()->y;
        return packet;
    }
    else {
     	cout << "packetShape() is not sending circles!" << endl;
        return packet;
    }
}

sf::Packet PacketParser::pack(player_info p)
{
	sf::Packet packet;
	packet << UDPNetwork::PLAYER_MOVE;
	packet << p.name;
	packet << p.movedir;
	return packet;
}

Shape* PacketParser::unpackageShape(sf::Packet& packet)
{
	b2Vec2 pos, size;
	if(!(packet >> pos.x >> pos.y >> size.x >> size.y))
		std::cerr << "Error occured in received data!" << endl;
	return factory.createRectangle(&size, &pos, true);
}

template<>
Shape* PacketParser::unpack<Shape>(sf::Packet& packet)
{
	b2Vec2 pos, size;
	packet >> pos.x >> pos.y >> size.x >> size.y;
	return factory.createRectangle(&size, &pos, true);
}

template<>
b2Vec2* PacketParser::unpack<b2Vec2>(sf::Packet& packet)
{
	b2Vec2 pos;
	packet >> pos.x >> pos.y;
	return new b2Vec2(pos);
}

template<>
int* PacketParser::unpack<int>(sf::Packet& packet)
{
	int i;
	packet >> i;
	return &i;
}

template<>
player_info* PacketParser::unpack<player_info>(sf::Packet& packet)
{
	player_info* info = new player_info;
	packet >> info->name;
	packet >> info->movedir;
	return info;
}