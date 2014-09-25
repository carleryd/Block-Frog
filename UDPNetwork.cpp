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

int UDPNetwork::receive(sf::Packet* p, sf::IpAddress& a, unsigned short& port)
{
	cout << "Packet received from " << a.getPublicAddress().toString() << " at port "<< port << endl;
	int r = mySocket.receive(*p, a, port);
	if(port == mySocket.getLocalPort() && a == sf::IpAddress::getPublicAddress())
	{
		cout << "FATAL ERROR: SERVER AND CLIENT USING SAME PORT" << endl;
		cout << "Trying to recover by removing from remote connections list" << endl;
	}
	return r;
}

int UDPNetwork::receive(sf::Packet* p)
{
	unsigned short port;
    sf::IpAddress pointlessIP;
    
	return receive(p, pointlessIP, port);
}

void UDPNetwork::listen()
{
	while(selector.wait())
	{
		if(selector.isReady(mySocket) && !packetsOccupied)
		{
			packetsOccupied = true;
			packets.push_front(new sf::Packet());
			receive(packets.back());
			cout << "Packet size: " << packets.front()->getDataSize() << endl;
			packetsOccupied = false;
		}
	}
}

void UDPNetwork::handleReceivedData(Game* game)
{
	while(!packets.empty() && !packetsOccupied)
	{
		packetsOccupied = true;
		sf::Packet* packet = packets.front();
		int type;
		*packet >> type;
		switch (type)
		{
		case SERVER_EXIT:
			game->getWindow()->close();
			break;
		case CLIENT_EXIT:
			break;
		case SHAPE:
			game->boxes.push_back(packetParser.unpackageShape(*packet));
			break;
		default:
			cerr << "Type " << type << " is not a recognized data type!" << endl;
			break;
		}

		packets.pop_front();
		packetsOccupied = false;
	}
}