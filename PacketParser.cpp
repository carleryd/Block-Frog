#include <iostream>
#include <typeinfo>

#include "Game.h"
#include "PacketParser.h"
#include "ShapeFactory.h"
#include "UDPNetwork.h"
#include "Rectangle.h"
#include "Item.h"

PacketParser::PacketParser(Game* game)
{
    factory = game->getShapeFactory();
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
		packet << shape->getDynamic();
        // This is some fancy-ugly stuff to convert 64 bit pointer value to 32 bit primitive int
        long long temp = reinterpret_cast<long long>(shape->getBody()->GetFixtureList()->GetUserData());
        int collisionID = static_cast<int>(temp);
        packet << collisionID;
		//if it is an item or not
		packet << (dynamic_cast<Item*>(shape) != nullptr? true : false);
        
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
	packet << p->localID;
	packet << p->movedir;
	packet << p->jumped;
	packet << p->velocity.x << p->velocity.y;
	packet << p->position.x << p->position.y;
	return packet;
}

sf::Packet PacketParser::pack(Player* p)
{
	sf::Packet packet;
	packet << UDPNetwork::NEW_PLAYER;
//	packet << p->getBody()->GetPosition().x << p->getBody()->GetPosition().y;
//	packet << p->getName();
	return packet;
}

sf::Packet PacketParser::pack(shapeSync* s, bool appendee)
{
	sf::Packet p;
	if(!appendee)
		p << UDPNetwork::SHAPE_SYNCH;
	p << s->shapeID;
	p << s->angularVel;
	p << s->velocity.x << s->velocity.y;
	p << s->position.x << s->position.y;
	p << s->angle;
//	p << s->collisionID;
	return p;
}

sf::Packet PacketParser::pack(lp_info* lp) {
    sf::Packet packet;
    int type = UDPNetwork::NEW_LOBBY_PLAYER;
    packet << type;
    packet << lp->ID;
    packet << lp->alias;
    packet << lp->color;
    return packet;
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
sf::Packet PacketParser::pack(int type)
{
	sf::Packet p;
	p << type;
	return p;
}

sf::Packet PacketParser::pack(int type, hook_info& h)
{
	sf::Packet p;
	p << type;
	p << h.localID;
	p << h.mousePos.x << h.mousePos.y;
	return p;
}

// ############# UNPACK FUNCTIONS ################
template<>
Shape* PacketParser::unpack<Shape*>(sf::Packet& packet)
{
	int id, collisionID;
	bool dynamic, item;
	b2Vec2 pos, size;
	packet >> pos.x >> pos.y >> size.x >> size.y;
	packet >> id >> dynamic >> collisionID >> item;
	Shape* s;
	if(!item)
	{
		s = factory->createClientRectangle(new b2Vec2(size), new b2Vec2(pos), dynamic, id, (uintptr_t)collisionID);
		//cout << "regular shape created" << endl;
	}
	else
	{
        cout << "Item " << id << " created at pos: " << pos.x << " " << pos.y << endl;
		s = factory->createItem(new b2Vec2(pos), id); // Needs collisionID, cba to add it :>
		//cout << "item created" << endl;
	}
	s->setPosition(&pos);
	s->setId(id);
	return s;
}

//template<>
//b2Vec2* PacketParser::unpack<b2Vec2*>(sf::Packet& packet)
//{
//	b2Vec2 pos;
//	packet >> pos.x >> pos.y;
//	return new b2Vec2(pos);
//}

template<>
int PacketParser::unpack<int>(sf::Packet& packet)
{
	int i;
	packet >> i;
	return i;
}

template<>
bool PacketParser::unpack<bool>(sf::Packet& packet)
{
	bool b;
	packet >> b;
	return b;
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
	p >> s->angularVel;
	p >> s->velocity.x >> s->velocity.y;
	p >> s->position.x >> s->position.y;
	p >> s->angle;
//	p >> s->collisionID;
	return s;
}

template<>
player_info* PacketParser::unpack<player_info*>(sf::Packet& packet)
{
	player_info* info = new player_info;
	packet >> info->localID;
	packet >> info->movedir;
	packet >> info->jumped;
	packet >> info->velocity.x >> info->velocity.y;
	packet >> info->position.x >> info->position.y;
	//hooktip
	/*shapeSync* hooktip = unpack<shapeSync*>(packet);
	info->hookTip = *hooktip;
	delete hooktip;
	//hookbase
	shapeSync* hookbase = unpack<shapeSync*>(packet);
	info->hookBase  = *hookbase;
	delete hookbase;*/
	return info;
}

template<>
hook_info* PacketParser::unpack<hook_info*>(sf::Packet& p)
{
	hook_info* h = new hook_info;
	p >> h->localID;
	p >> h->mousePos.x >> h->mousePos.y;
	return h;
}

template<>
res_info* PacketParser::unpack<res_info*>(sf::Packet& p)
{
    res_info* resInfo = new res_info;
    p >> resInfo->localID;
    p >> resInfo->spawn.x;
    p >> resInfo->spawn.y;
    return resInfo;
}

template<>
lp_info* PacketParser::unpack<lp_info*>(sf::Packet& p)
{
 	lp_info* lpInfo = new lp_info;
    p >> lpInfo->ID;
    p >> lpInfo->alias;
    p >> lpInfo->color;
    return lpInfo;
}

