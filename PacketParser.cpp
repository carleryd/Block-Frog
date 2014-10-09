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

// ################## PACK FUNCTIONS ##########################

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

sf::Packet PacketParser::pack(Player* p)
{
	sf::Packet packet;
	packet << UDPNetwork::NEW_PLAYER;
	packet << p->getPosition()->x << p->getPosition()->y;
	packet << p->getName();
	return packet;
}

sf::Packet PacketParser::pack(shapeSync* s)
{
	sf::Packet p;
	p << UDPNetwork::SHAPE_SYNCH;
	p << s->shapeID;
	p << s->angularVel;
	p << s->velocity.x << s->velocity.y;
	return p;
}

// ############# UNPACK FUNCTIONS ################
template<>
Shape* PacketParser::unpack<Shape*>(sf::Packet& packet)
{
	b2Vec2 pos, size;
	packet >> pos.x >> pos.y >> size.x >> size.y;
	Shape* s = factory.createRectangle(new b2Vec2(size), new b2Vec2(pos), true);
	s->setPosition(&pos);
	return s;
}

template<>
b2Vec2* PacketParser::unpack<b2Vec2*>(sf::Packet& packet)
{
	b2Vec2 pos;
	packet >> pos.x >> pos.y;
	return new b2Vec2(pos);
}

template<>
int PacketParser::unpack<int>(sf::Packet& packet)
{
	int i;
	packet >> i;
	return i;
}

template<>
string PacketParser::unpack<string>(sf::Packet& packet)
{
	string s;
	packet >> s;
	return s;
}

template<>
player_info* PacketParser::unpack<player_info*>(sf::Packet& packet)
{
	player_info* info = new player_info;
	packet >> info->name;
	packet >> info->movedir;
	return info;
}

template<> 
shapeSync* PacketParser::unpack<shapeSync*>(sf::Packet& p)
{
	shapeSync* s = new shapeSync;
	p >> s->shapeID;
	p >> s->angularVel;
	p >> s->velocity.x >> s->velocity.y;
	return s;
}