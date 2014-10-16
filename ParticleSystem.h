#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <SFML\Graphics.hpp>

class ParticleSystem: public sf::Drawable, public sf::Transformable
{
public:
	ParticleSystem(int count, sf::Vector2f position);
	~ParticleSystem(void);

	void setEmitter( sf::Vector2f position);
	void update();
	bool isRunning() {return running;};
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void resetParticle(size_t index);

    sf::VertexArray vertices;
    sf::Time lifetime;
    sf::Vector2f emitter;
	sf::Clock clock;
	sf::Clock lifeClock;
	bool running;
	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifeTime;
	};
    std::vector<Particle> particles;
};

#endif