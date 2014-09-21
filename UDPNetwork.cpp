#include "UDPNetwork.h"
#include <iostream>
#include "Game.h"

UDPNetwork::UDPNetwork(string PlayerName, ShapeFactory& factory):
	packetParser(factory)
{
	if(mySocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		cerr << "Could not bind port" << endl;
	else
	{
		cout << "My IP address: "<< myAddress.getPublicAddress().toString() << endl;
		cout << "My port: " << mySocket.getLocalPort() << endl;
	}
	playerName = PlayerName;
	selector.add(mySocket);
}

UDPNetwork::~UDPNetwork(void)
{
}

int UDPNetwork::send(sf::Packet& p, sf::IpAddress& a, unsigned short& port)
{
	cout << "Packet sent." << endl;
	return mySocket.send(p, a, port);
}

int UDPNetwork::receive(sf::Packet& p, sf::IpAddress& a, unsigned short& port)
{
	cout << "Packet received." << endl;
	return mySocket.receive(p, a, port);
}

int UDPNetwork::receive(sf::Packet& p)
{
	cout << "Packet recevied." << endl;
	unsigned short port;
	return mySocket.receive(p, sf::IpAddress(), port);
}

void UDPNetwork::listen()
{
	sf::Packet packet;
	while(selector.wait())
	{
		packet = sf::Packet();
		if(selector.isReady(mySocket))
		{
			receive(packet);
			packets.push_back(&packet);
		}
	}
}

void UDPNetwork::handleReceivedData(Game* game)
{
	while(!packets.empty())
	{
		sf::Packet& packet = *packets.front();
		int type;
		packet >> type;
		switch (type)
		{
		case SHAPE:
			game->boxes.push_back(packetParser.unpackageShape(packet));
			break;
		default:
			cerr << "Type " << type << " is not a recognized data type!" << endl;
			break;
		}

		packets.pop_front();
	}
}