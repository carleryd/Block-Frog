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
		packet << shape->getId();
        return packet;
    }
    else {
     	cout << "packetShape() is not sending circles!" << endl;
        return packet;
    }
}

template<>
sf::Packet PacketParser::pack<player_info*>(int type, player_info* p)
{
	sf::Packet packet;
	packet << type;
	packet << p->name;
	packet << p->movedir;
	packet << p->jumped;
	packet << p->velocity.x << p->velocity.y;
	packet << p->position.x << p->position.y;
//	sf::Packet ht = pack(&p->hookTip, true);
//	sf::Packet hb = pack(&p->hookBase, true);
//	packet.append(ht.getData(), ht.getDataSize());
//	packet.append(hb.getData(), ht.getDataSize());
	return packet;
}

sf::Packet PacketParser::pack(Player* p)
{
	sf::Packet packet;
	packet << UDPNetwork::NEW_PLAYER;
	packet << p->getBody()->GetPosition().x << p->getBody()->GetPosition().y;
	packet << p->getName();
	return packet;
}

sf::Packet PacketParser::pack(shapeSync* s, bool appendee)
{
	sf::Packet p;
	if(!appendee)
		p << UDPNetwork::SHAPE_SYNCH;
	p << s->shapeID;
//	p << s->angularVel;
//	p << s->velocity.x << s->velocity.y;
	p << s->position.x << s->position.y;
	p << s->angle;
//	p << s->hookUserData;
	return p;
}

template<>
sf::Packet PacketParser::pack<int>(int type, int value)
{
	sf::Packet p;
	p << type;
	p << value;
	return p;
}

template<>
sf::Packet PacketParser::pack<string>(int type, string s)
{
	sf::Packet p;
	p << type;
	p << s;
	return p;
}
//sf::Packet PacketParser::pack

// ############# UNPACK FUNCTIONS ################
template<>
Shape* PacketParser::unpack<Shape*>(sf::Packet& packet)
{
	int id;
	b2Vec2 pos, size;
	packet >> pos.x >> pos.y >> size.x >> size.y;
	packet >> id;
	Shape* s = factory.createRectangle(new b2Vec2(size), new b2Vec2(pos), true, id);
	s->setPosition(&pos);
	s->setId(id);
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
shapeSync* PacketParser::unpack<shapeSync*>(sf::Packet& p)
{
	shapeSync* s = new shapeSync;
	p >> s->shapeID;
//	p >> s->angularVel;
//	p >> s->velocity.x >> s->velocity.y;
	p >> s->position.x >> s->position.y;
	p >> s->angle;
//	p >> s->hookUserData;
	return s;
}

template<>
player_info* PacketParser::unpack<player_info*>(sf::Packet& packet)
{
	player_info* info = new player_info;
	packet >> info->name;
	packet >> info->movedir;
	packet >> info->jumped;
	packet >> info->velocity.x >> info->velocity.y;
	packet >> info->position.x >> info->position.y;
	//hooktip
//	shapeSync* hooktip = unpack<shapeSync*>(packet);
//	info->hookTip = *hooktip;
//	delete hooktip;
	//hookbase
//	shapeSync* hookbase = unpack<shapeSync*>(packet);
//	info->hookBase  = *hookbase;
//	delete hookbase;
	return info;
}

