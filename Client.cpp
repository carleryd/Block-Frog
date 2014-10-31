#include "Client.h"
#include <iostream>
#include <typeinfo>

Client::Client(Game* game, sf::IpAddress sAddress, unsigned short port):
	UDPNetwork(game), serverAddress(sAddress), serverPort(port)
{
	//cout << (connect() ? "Connected to "  : "Failed to connect to " ) << sAddress.toString() << endl;
	selector.add(mySocket);
}


Client::~Client(void)
{
}

bool Client::isServer()
{
	return false;
}

bool Client::connect()
{
	cout << "contacting server" << endl;
	//send
	sf::Packet p;
	p << UDPNetwork::NEW_PLAYER;
	send(p, serverAddress, serverPort);
	p.clear();
	
	//receive
	cout << "Waiting for host response" << endl;
	int ret = receive(&p);
    string message;
	p >> message;
	
	cout << "Message received from host: " << message << endl;
	return ret == sf::Socket::Done ? true : false;
}
void Client::sendToServer(sf::Packet& p)
{
	send(p, serverAddress, serverPort);
}

void Client::dropServer()
{
	serverAddress = NULL;
	serverPort = 0;
	cout << "Server dropped." << endl;
}