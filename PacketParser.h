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

struct shapeSync
{
	int shapeID;
	float angularVel;
	b2Vec2 velocity;
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
	
	//template function SHOULD be able to back ONE of any object
	template<typename T>
		T unpack(sf::Packet& packet)
		/*{
			T variable;
			static_assert(std::is_fundamental<T>::value, "FATAL ERROR: WRONG TEMPLATE ARGUMENT TYPE; PacketParser::unpack<T>(sf::packet& p)");
			packet >> variable;
			return variable;
		}*/;

private:
	ShapeFactory& factory;
};

/*sf::Packet& operator<<(sf::Packet& appendto, const sf::Packet appendwith)
{
	appendto.append(appendwith.getData(), appendwith.getDataSize());
	return appendto;
};*/

#endif