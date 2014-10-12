#include "Textor.h"
#include "OSHandler.h"
#include <iostream>
using namespace std;

Textor::Textor(OSHandler* os)
{
	if (!font.loadFromFile(os->getResourcePath() + "sansation.ttf")) {
        cerr << "Could not load " << os->getResourcePath() + "sansation.ttf" << endl;
    }
	text.setFont(font);
	text.setColor(sf::Color(255, 100, 255));
}


Textor::~Textor(void)
{
}

sf::Text Textor::write(string _text, sf::Vector2f pos)
{
	text.setString(_text);
	text.setPosition(pos);
	text.setOrigin(text.getGlobalBounds().width/2, text.getGlobalBounds().height/2);
	return text;
}