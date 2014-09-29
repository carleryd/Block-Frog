#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Box2D/Box2D.h>
#include "Shape.h"
#include <random>
using namespace std;

class Game;

class ShapeFactory
{
public:
	ShapeFactory(Game* game);
	~ShapeFactory(void);

	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic);
	Shape* createRandomShape(sf::Vector2i viewOffset);

	template<class T>
	b2Vec2* sfvec_to_b2vec(sf::Vector2<T>& v)
	{
		sf::Vector2<T> adjustVector = sf::Vector2<T>(v.x - game->getWindow()->getSize().x/2, 
			-v.y + game->getWindow()->getSize().y/2);
		adjustVector += sf::Vector2<T>(game->getViewOffset());
		return new b2Vec2(float(adjustVector.x), float(adjustVector.y));
			/*new b2Vec2(float(v.x - game->getWindow()->getSize().x/2 + game->getViewOffset().x),
			float(-v.y + game->getWindow()->getSize().y/2 + game->getViewOffset().y));*/
	};

private:
	//b2Vec2 convert_sf_vector(sf::Vector2i v);

	Game* game;
	mt19937 mersenneGen;
	uniform_real_distribution<float> dist;
	int minSize;
};
#endif