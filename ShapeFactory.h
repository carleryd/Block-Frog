#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Box2D\Box2D.h>
#include "Shape.h"
#include <random>
using namespace std;

class ShapeFactory
{
public:
	ShapeFactory(b2World* world, sf::RenderWindow* window);
	~ShapeFactory(void);

	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic);
	Shape* createRandomShape(sf::Vector2i viewOffset);

	template<class T>
	b2Vec2* sfvec_to_b2vec(sf::Vector2<T> v)
	{
		return new b2Vec2(float(v.x - window->getSize().x/2), 
			float(-v.y + window->getSize().y/2));
	};

private:
	//b2Vec2 convert_sf_vector(sf::Vector2i v);

	b2World* world;
	sf::RenderWindow* window;
	mt19937 mersenneGen;
	uniform_real_distribution<float> dist;
	int minSize;
};
#endif