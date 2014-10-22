#include "Textor.h"
#include "OSHandler.h"
#include <iostream>
#include <stack>
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
    cout << "Destructor not implemented" << endl;
}

sf::Text Textor::write(string text_, sf::Vector2f pos)
{
	text.setString(text_);
    
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top  + textRect.height/2.0f);
    
    text.setPosition(pos);
	return text;
}

void Textor::writeTemporaryText(string t, sf::Vector2f p, int l)
{
	temporaryTexts.push_back(tempText());
	temporaryTexts.back().t = write(t, p);
	temporaryTexts.back().secondsLasting = l;
}

void Textor::handleTemporaryTexts(sf::RenderWindow& w)
{
	for (tempText tt: temporaryTexts)
	{
		w.draw(tt.t);
	}
	auto remove = remove_if(temporaryTexts.begin(), temporaryTexts.end(), [](tempText tt)  
		{
			return tt.c.getElapsedTime().asSeconds() > tt.secondsLasting;
		});
	if(remove != temporaryTexts.end())
	{
		temporaryTexts.erase(remove);
	}
}