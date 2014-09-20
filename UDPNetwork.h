#ifndef  UDPNETWORK_H
#define UDPNETWORK_H
#include <SFML/Network.hpp>
#include <vector>

using namespace std;

class UDPNetwork
{
public:
	UDPNetwork(void);
	~UDPNetwork(void);
	int send(sf::Packet& packet, sf::IpAddress remoteAddress, unsigned short remotePort);
	int receive(sf::Packet& packet, sf::IpAddress remoteAddress, unsigned short remotePort);
protected:
	sf::UdpSocket mySocket;
	sf::IpAddress myAddress;
};

#endif