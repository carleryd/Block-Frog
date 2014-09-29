#include "Game.h"
#include "OSHandler.h"
#include "Server.h"
#include "Client.h"
#include "ShapeFactory.h"

using namespace std;

Game::Game(sf::RenderWindow* window_, OSHandler* osHandler_, int playerType, sf::IpAddress* serverAddress, unsigned short serverPort)
{
    window = window_;
    osHandler = osHandler_;
    
	view = new sf::View();
	view->setCenter((float)window->getSize().x/2, (float)window->getSize().y/2);
	view->setSize(sf::Vector2f(window->getSize()));
	window->setView(*view);
    
    // Setting up Box2D physics
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    world = new b2World(gravity);
    
	//shapefactory for creating shapes easily
	shapeFactory = new ShapeFactory(this);
    
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f),
                                                  new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                                  false)
                    );
    
    player = new Player(this);

	riseSpeed = 0; //-0.2f;
	killOffset = 30;
	secPerDrops = 1;
	allowJoin = true;

	//networking
	switch (playerType)
	{
	case SERVER:
		localHost = new Server("HOST", *shapeFactory);
		join = new thread(&Server::waitForPlayers, dynamic_cast<Server*>(localHost), std::ref(allowJoin));
		//dynamic_cast<Server*>(localHost)->waitForPlayers();
		cout << "Waiting for players..." << endl;
		window->setTitle("SERVER");
		break;
	case CLIENT:
		localHost = new Client("CLIENT", *serverAddress, serverPort, *shapeFactory);
		if(!dynamic_cast<Client*>(localHost)->connect())
		{
			cerr << "Failed to connect to "<< serverAddress->toString() << endl;
		}
		network = new thread(&UDPNetwork::listen, localHost);
		window->setTitle("CLIENT");
		break;
	case SINGLE_PLAYER:
		localHost = new Server("HOST", *shapeFactory);
		window->setTitle("SINGLE PLAYER");
		break;
	default:
		break;
	}

	
	window->setActive(true);
	
	// 
	secPerDrops = 5;
}

Game::~Game()
{
	while(!boxes.empty())
	{
		delete boxes.back();
		boxes.pop_back();
	}
	delete world;
	delete player;
	delete view;
	delete shapeFactory;
	network->~thread();
	join->~thread();
}

sf::RenderWindow* Game::getWindow() {
    return window;
}

b2World* Game::getWorld() {
    return world;
}

Player* Game::getPlayer() {
    return player;
}

OSHandler* Game::getOSHandler() {
    return osHandler;
}

void Game::run() {
	//Handle data that was received since last timestep
	localHost->handleReceivedData(this);

	handleThreads();
	boxHandling();
	calcViewOffset();


	if(localHost->isServer())
	{
		Shape* box = createBoxes();
		dynamic_cast<Server*>(localHost)->broadCast(box);
	}
	//else 
	//{
	//	boxes.push_back( static_cast<Shape*>(dynamic_cast<Client*>(localHost)->listenToServer()));
	//}

	//player
    player->draw();
	player->updatePlayer();

	//move view up/raise water level
	view->move(0, riseSpeed);
	window->setView(*view);
    
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 8);
    world->ClearForces();

	//do  networking that has happened during this frame
	//e.g. send data that player has moved a block


	//rebroadcast everything that server has learnt from the other clients about the game state
	if(localHost->isServer())
	{
		//localHost->handleReceivedData(this);
		//rebroadcast
	}
}

void Game::spawnBox(sf::Vector2i position) {
	//add view offset
	position += viewOffset;
    // adjPos is a position adjusted to window position, also adjusted according to Box2D positions, y upwards
    sf::Vector2i adjPos = sf::Vector2i(position.x - window->getSize().x/2,
		-position.y + window->getSize().y/2);

	boxes.push_back(
		shapeFactory->createRectangle(
			new b2Vec2(20.0f, 20.0f),
			new b2Vec2(float(adjPos.x), float(adjPos.y)),
			true));
}

void Game::removeFallenBoxes(list<Shape*>& deletion)
{
	//if(!deletion.empty())
	//	cout << deletion.size() << " shapes have fallen out of the screen and been deleted this frame." << endl;
	while(!deletion.empty())
	{
		vector<Shape*>::iterator todelete = std::remove_if(boxes.begin(), boxes.end(), [&deletion](Shape* b)
		{
			return deletion.front() == b;
		}
		);
		boxes.erase(todelete, boxes.end());
		delete deletion.front();
		deletion.pop_front();
	}
	deletion.clear();
}

void Game::calcViewOffset()
{
	sf::Vector2f halfExtents = view->getSize() / 2.0f;
	sf::Vector2f translation = view->getCenter() - halfExtents;
	viewOffset.x = (int)translation.x;
	viewOffset.y = (int)translation.y;
}

void Game::boxHandling()
{
	//handle boxes
	list<Shape*> deletion;
    for(Shape* box : boxes) 
	{
		//check if box has fallen "outside"
		if(box->getShape()->getPosition().y > window->getSize().y - viewOffset.y + killOffset)
			deletion.push_back(box);
        box->update();
        window->draw(*box->getShape());
    }
	removeFallenBoxes(deletion);
}

Shape* Game::createBoxes()
{
	//random dropping of boxes
    timer = clock.getElapsedTime();
    duration = timer.asSeconds();
    
	if(duration > secPerDrops)
	{
		boxes.push_back(shapeFactory->createRandomShape(viewOffset));
		duration = 0;
        clock.restart();
		return boxes.back();
	}	
	else
		return nullptr;
}

void Game::handleThreads()
{
	switch (localHost->isServer())
	{
	case true:
		if(!allowJoin)
		{
			join->join();
			network = new thread(&UDPNetwork::listen, localHost);
			cout << "No more players allowed yo join." << endl;
		} 
		break;
	case false:
		network = new thread(&UDPNetwork::listen, localHost);
		break;
	default:
		break;
	}

}