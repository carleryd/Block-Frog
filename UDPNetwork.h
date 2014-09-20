#ifndef  UDPNETWORK_H
#define UDPNETWORK_H
#include <SFML/Network.hpp>
#include <vector>

using namespace std;

class UDPNetwork
{
public:
	~UDPNetwork();
	int send(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);
	int receive(sf::Packet& packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);
	int receive(sf::Packet& packet);
	virtual bool isServer() = 0;
	sf::IpAddress getMyAddress() const{return myAddress;};
	unsigned short getMyPort() const{return mySocket.getLocalPort();};
protected:
	UDPNetwork(string playerName);

	sf::UdpSocket mySocket;
	sf::IpAddress myAddress;
	std::string playerName;
};

#endif