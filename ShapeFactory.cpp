#include "ShapeFactory.h"
#include "Rectangle.h"
#include <functional>


ShapeFactory::ShapeFactory(b2World* world, sf::RenderWindow* window):
	world(world), window(window), dist(0.0, 1.0)
{
	string s("Block frog");
	seed_seq seed(s.begin(), s.end());
	mersenneGen.seed(seed);
	minSize = 20;
}


ShapeFactory::~ShapeFactory(void)
{

}

Shape* ShapeFactory::createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic)
{
	return new Rectangle(world, size, position, window, dynamic);
}

Shape* ShapeFactory::createRandomShape(sf::Vector2i viewOffset)
{
	auto rand = bind(dist, mersenneGen);
	float x = float(dist(mersenneGen) * window->getSize().x + viewOffset.x);
	float y = float(-200 + viewOffset.y); //remember that positive y is up in box2d

	return new Rectangle(
		world, 
		new b2Vec2(rand()*100 + minSize, rand()*100 + minSize), 
		sfvec_to_b2vec(sf::Vector2f(x, y)), 
		window, 
		true);
}


