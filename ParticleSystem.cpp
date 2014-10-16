#include "ParticleSystem.h"
#include <math.h>


ParticleSystem::ParticleSystem(int count, sf::Vector2f pos):
	particles(count), vertices(sf::Points, count), lifetime(sf::seconds(3)), emitter(0,0)
{
	emitter = pos;
	running = true;
	for (size_t i = 0; i < particles.size(); ++i)
	{
		/*particles[i].lifeTime = sf::milliseconds((std::rand() % 2000) + 1000);
		//particles[i].velocity = sf::Vector2f(0,0);*/
		resetParticle(i);
	}
}

void ParticleSystem::setEmitter( sf::Vector2f position)
{
	emitter = position;
}

ParticleSystem::~ParticleSystem(void)
{
}


void ParticleSystem::update()
{
	sf::Time elapsed = clock.restart();
	for (size_t i = 0; i < particles.size(); ++i)
	{
		//update particle lifetime
		Particle& p = particles[i];
		p.lifeTime -= elapsed;

		//if the particle is dead, respawn it
		if(p.lifeTime <= sf::Time::Zero)
			running = false;

		//update the position of the corresponding vertex
		vertices[i].position += p.velocity * elapsed.asSeconds();
		//affect speed with gravity
		p.velocity.y += 6; // elapsed.asSeconds();

		//update the alpha of the particle according to its lifetime
		float ratio = p.lifeTime.asSeconds() / lifetime.asSeconds();
		vertices[i].color = sf::Color::White;
		vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
	}
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//apply transform
	states.transform *= getTransform();

	//our particles don't use a texture
	states.texture = NULL;

	//draw the vertex array
	target.draw(vertices, states);
}

void ParticleSystem::resetParticle(size_t index)
{
    // give a random velocity and lifetime to the particle
    float angle = (std::rand() % 360) * 3.14f / 180.f;
    float speed = (std::rand() % 50) + 150.f;
    particles[index].velocity = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);
    particles[index].lifeTime = sf::milliseconds((std::rand() % 2000) + 1000);

    // reset the position of the corresponding vertex
    vertices[index].position = emitter;
}