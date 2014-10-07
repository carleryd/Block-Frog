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

sf::Packet PacketParser::pack(Shape* shape, int type_ = UDPNetwork::SHAPE)
{
	if((type_ != UDPNetwork::SHAPE) &&  (type_ != UDPNetwork::SHAPE_SYNCH))
	{
		cerr << "ERROR: Wrong type packeted! Type used:" << type_ << ". Should be SHAPE or SHAPE_SYNCH" << endl;
		return sf::Packet();
	}
    Rectangle* rectangle = dynamic_cast<Rectangle*>(shape);
    sf::Packet packet;
    
    if(rectangle != nullptr) {
        b2Body* body = rectangle->getBody();
        int type = type_;
        packet << type; //type
        packet << body->GetPosition().x << body->GetPosition().y;
        packet << rectangle->getSize()->x << rectangle->getSize()->y;
		packet << rectangle->getVelocity().x << rectangle->getVelocity().y;
		packet << body->GetAngle() << body->GetAngularVelocity();
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

// ############# UNPACK FUNCTIONS ################
template<>
Shape* PacketParser::unpack<Shape*>(sf::Packet& packet)
{
	b2Vec2 pos, size, velocity;
	float angle, angularvelocity;
	packet >> pos.x >> pos.y;
	packet >> size.x >> size.y;
	packet >> velocity.x >> velocity.y;
	packet >> angle >> angularvelocity;
	//cout << "pos x: " << pos.x << ", " << pos.y << endl;
	Shape* s =  factory.createRectangle(new b2Vec2(size), new b2Vec2(pos), true);
	s->setPosition(&pos, angle);
	s->setVelocity(velocity);
	s->setAngularVelocity(angularvelocity);
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

