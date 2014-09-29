#include "PacketParser.h"
//#include "Shape.h"
#include "ShapeFactory.h"
#include "UDPNetwork.h"
#include <iostream>
#include "Rectangle.h"

PacketParser::PacketParser(ShapeFactory& f):
	factory(f)
{
}


PacketParser::~PacketParser(void)
{
}

sf::Packet PacketParser::packageShape(Shape* shape)
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

Shape* PacketParser::unpackageShape(sf::Packet& packet)
{
	int type;
	b2Vec2 pos, size;
	if(!(packet >> pos.x >> pos.y >> size.x >> size.y))
		std::cerr << "Error occured in received data!" << endl;
	return factory.createRectangle(&size, &pos, true);
}