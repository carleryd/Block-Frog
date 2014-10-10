#ifndef TEXTOR_H
#define TEXTOR_H
#include <SFML\Graphics.hpp>
using namespace std;

class OSHandler;

class Textor
{
public:
	Textor(OSHandler* os);
	~Textor(void);

	sf::Text write(string text, sf::Vector2f position);
private:
	sf::Font font;
	sf::Text text;
};

#endif