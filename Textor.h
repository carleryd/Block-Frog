#ifndef TEXTOR_H
#define TEXTOR_H
#include <SFML/Graphics.hpp>
#include <list>
using namespace std;

class OSHandler;


struct tempText
{
	sf::Text t;
	sf::Clock c;
	int secondsLasting;
};

class Textor
{
public:
	Textor(OSHandler* os);
	~Textor(void);

	sf::Text write(string text, sf::Vector2f position);
	void writeTemporaryText(string text, sf::Vector2f position, int secondsLasting);
	//print text, remove if time has run out.
	void handleTemporaryTexts(sf::RenderWindow& w);
private:
	sf::Font font;
	sf::Text text;
	list<tempText> temporaryTexts;
};


#endif