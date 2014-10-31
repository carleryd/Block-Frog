#include "Game.h"
#include "OSHandler.h"
#include "Server.h"
#include "Client.h"
#include "ShapeFactory.h"
#include "Utility.h"
#include "Textor.h"
#include "ContactListener.h"
#include "Remover.h"
#include "Synchronizer.h"
#include "Lobby.h"

using namespace std;

Game::Game(sf::RenderWindow* window_, OSHandler* osHandler_)
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
	water = new sf::RectangleShape(sf::Vector2f(float(window->getSize().x * 2), float(window->getSize().y)));
	water->setFillColor(sf::Color(255, 0, 0, 150));
    // Need to set it here aswell as in run() because otherwise it is set to 0 at start, resulting in
    // removal of all shapes created before it is set in run().
    water->setPosition(float(window->getSize().x * -0.1),
                       float(viewOffset.y + water->getLocalBounds().height - 10));
	textor = new Textor(osHandler_);
	// This is set in Lobby::Lobby(...)
    lobby = nullptr;
    // This is set in Director::Director(...)
    director = nullptr;
    
	exitCalled = false;
    gameHasStarted = false;
	
    platformsPerLevel = 1.5;
    arrangeX = 1;
    prevArrangeX = 1;
//	staticPlatform = 0;
    prevStage = 0;
    waterStage = 1;
    score = 0;
    boxCount = 0;
	localHost = NULL;
    prepTime = 30;
    
    // Used by client to set position of given water levels
    oldViewOffset = sf::Vector2i(0, 0);
    
    shapeFactory = new ShapeFactory(this);
    utility = new Utility(this);
	synchronizer = new Synchronizer(*this);
    
    contactListener = new ContactListener(this);
    world->SetContactListener(contactListener);
}

Game::~Game()
{
	while(!boxes.empty())
	{
		delete boxes.back();
		boxes.pop_back();
	}
	while(!remotePlayers.empty())
	{
		delete remotePlayers.back();
		remotePlayers.pop_back();
	}
	delete world;
	delete player;
	delete view;
	delete shapeFactory;
	exitGame();
}

void Game::destroyPlayer() {
    delete player;
    if(player == nullptr) cout << "player is nullptr" << endl;
	else {
     	player = nullptr;
        cout << "player needs to be set nullptr" << endl;
    }
}

void Game::init() {
	player = new Player(this, localID, lobby->getColors()[localID-1]);
   	player->init();
    cout << "THIS MACHINE LOCALID: " << localID << endl;
    
    // Add all players from lobby
	for(LobbyPlayer lp : lobby->getConnected()) {
        if(lp.ID != localID) {
	        Player* newPlayer = new Player(this, lp.ID, lp.color);
            cout << "Remote player: " << newPlayer->getLocalID() << " " << lp.color << endl;
    	    addRemotePlayer(newPlayer);
        }
    }
    
	packetParser = new PacketParser(this);
    
	//ground
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(1280.0f, 50.0f),
                                                  new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                                  false,
                                                  1.0,
                                                  1.0)
                    );
    
    
	lastStaticShape = boxes.back();
    
	
	//test shape
    //boxes.push_back(shapeFactory->createRandomShape(viewOffset));

	rise = false;
	riseSpeed = -0.2f;
	allowJoin = true;
	remover = new Remover(this);
	
    if(localHost->isServer())
	    createStaticPlatform();
    
	window->setActive(true);
	
    // Needed so that preparation time is not started on Game::Game(...)
    clock.restart();
	secPerDrops = 3;
}

void Game::createStaticPlatform() {
    b2Vec2* size = new b2Vec2(window->getSize().x/3, 50);
    
    do {
        arrangeX = rand() % 3;
    } while(arrangeX == prevArrangeX);
	prevArrangeX = arrangeX;
    
    b2Vec2* pos = new b2Vec2(size->x * arrangeX - window->getSize().x/3,
                             waterStage * window->getSize().y / platformsPerLevel - window->getSize().y/2);
    
    Shape* staticPlatform = shapeFactory->createRectangle(size, pos,
                                                  false,
                                                  1.0,
                                                  1.0,
                                                  2);
    
    boxes.push_back(staticPlatform);
    staticShapes.push_back(staticPlatform);
    
    // Send new static shape to clients
    sf::Packet packet = packetParser->pack(boxes.back());
    dynamic_cast<Server*>(localHost)->broadCast(packet);
    
    b2Vec2 itemPos = b2Vec2(staticShapes.back()->getBody()->GetPosition().x * utility->getMTP(),
                            staticShapes.back()->getBody()->GetPosition().y * utility->getMTP());
    
    itemPos.y += 10;
    Shape* item = shapeFactory->createItem(&itemPos);
    boxes.push_back(item);
    
    // Send new item to clients
    packet.clear();
    packet = packetParser->pack(item);
    dynamic_cast<Server*>(localHost)->broadCast(packet);
}

void Game::setUtility(Utility* utility_) { utility = utility_; }
void Game::setPlayer(Player* player_) { player = player_; }

void Game::run() {
	//Handle data that was received since last timestep
	localHost->handleReceivedData(this);

	boxHandling();
    if(localHost->isServer())
		calcViewOffset();
	textor->handleTemporaryTexts(*window);
	//playerBoxInteraction();

	//water
	water->setPosition(float(window->getSize().x * -0.1),
                       float(viewOffset.y + water->getLocalBounds().height - 10));
	window->draw(*water);
    
    
//    cout << "waterStage: " << waterStage << endl;
//    cout << "position: " << -(water->getPosition().y - window->getSize().y) << endl;
    if(localHost->isServer()) {
        waterStage = (-(water->getPosition().y - window->getSize().y) / (window->getSize().y / platformsPerLevel)) + 2;
        if(prevStage < waterStage) {
            createStaticPlatform();
            prevStage = waterStage;
            // Gradually increase speed per new platform
            riseSpeed = -0.05 * waterStage;
//            sf::Packet p = packetParser->pack(UDPNetwork::RISE_CHANGE);
//            p << riseSpeed;
            //            dynamic_cast<Server*>(localHost)->broadCast(p);
            
            if(staticShapes.size() > 1) lastStaticShape = staticShapes[staticShapes.size()-2];
            else cout << "WARNING: staticShapes not more than 1" << endl;
            
            sf::Packet packet = packetParser->pack(UDPNetwork::PLATFORM_CHANGE);
            packet << lastStaticShape->getId();
            dynamic_cast<Server*>(localHost)->broadCast(packet);
        }
    }
    
    timer = clock.getElapsedTime();
    if(!gameHasStarted) {
        if(localHost->isServer()) {
        	elapsedPrepTime = timer.asSeconds();
        	
            sf::Packet packet = packetParser->pack(UDPNetwork::PREPTIME_CHANGE);
        	packet << elapsedPrepTime;
        	dynamic_cast<Server*>(localHost)->broadCast(packet);
        }
        else
            synchronizer->requestGameHasStarted();
        
        string countDown = to_string(elapsedPrepTime - prepTime);
        countDown = countDown.substr(1, countDown.size());
        
        window->draw(textor->write(countDown, sf::Vector2f(window->getSize().x/2,
                                                           30+viewOffset.y)));
        if(elapsedPrepTime - prepTime >= 0) {
         	gameHasStarted = true;
            score = 0;
        }
    }
    else {
	    window->draw(textor->write("Score: " + to_string(score), sf::Vector2f(window->getSize().x/2,
                                                                              30+viewOffset.y)));
        
        startRise();
    }
    
    if(localHost->isServer())
    {
        Shape* box = createBoxes();
        if(box != nullptr)
        {
            sf::Packet packet = packetParser->pack(box);
            dynamic_cast<Server*>(localHost)->broadCast(packet);
//            if(staticPlatform == 0)
//            {
//                b2Vec2 v = lastStaticShape->getBody()->GetPosition();
//                v.y += 10;
//                boxes.push_back(shapeFactory->createItem(&v));
//                packet.clear();
//                packet = packetParser->pack(boxes.back());
//                dynamic_cast<Server*>(localHost)->broadCast(packet);
//            }
        }
    }

	//player
	if(player != nullptr) playerHandling();

	//move view up/raise water level
	if(localHost->isServer() && rise)
		view->move(0, riseSpeed);
    
//    view->setViewport();
    if(!localHost->isServer()) {
        view->move(-oldViewOffset.x, -oldViewOffset.y);
	    view->move(viewOffset.x, viewOffset.y);
        oldViewOffset = viewOffset;
    }
    
	window->setView(*view);
//    cout << "Current water level: " << viewOffset.y << endl;
    
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 8);
    world->ClearForces();

	//do  networking that has happened during this frame
	//e.g. send data that player has moved a block


	//rebroadcast everything that server has learnt from the other clients about the game state
	if(localHost->isServer())
	{
		Server* server = dynamic_cast<Server*>(localHost);

		//broadcast shapes that server has affected
		/*for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			server->broadCast(p);
		}*/

		//broadcast water level:
//        cout << "Sending water level: " << viewOffset.y << endl;
		sf::Packet p = packetParser->pack<int>(UDPNetwork::WATER_LEVEL, viewOffset.y);
		server->broadCast(p);
		sf::Packet score = packetParser->pack<int>(UDPNetwork::SCORE_CHANGE, Game::score);
		server->broadCast(score);
	}
	else
	{
		//send local changes to server
		/*for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			dynamic_cast<Client*>(localHost)->sendToServer(p);
		}*/
		//request server update on shapes
		synchronizer->requestShapeUpdates();
	}

	synchronizer->requestPlayerUpdates();
	//clear memory that is only valid this time step.
	while(!localChanges.empty())
	{
		delete localChanges.back();
		localChanges.pop_back();
	}
	
}

void Game::spawnBox(sf::Vector2i position) {
    // adjPos is a position adjusted to window position, also adjusted according to Box2D positions, y upwards
    sf::Vector2i adjPos = sf::Vector2i(position.x - window->getSize().x/2,
		-position.y + window->getSize().y/2);

	//shapeFactory->sfvec_to_b2vec(position),
	
	boxes.push_back(
		shapeFactory->createRectangle(
			new b2Vec2(20.0f, 20.0f),
			new b2Vec2(float(adjPos.x), float(adjPos.y)),
			true));
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
    for(Shape* box : boxes) 
	{
        box->update();
        window->draw(*box->getShape());
		//check if box has fallen "outside"
		if(localHost->isServer())
			remover->checkShapeRemoval(box);
    }

	if(localHost->isServer())
		remover->removeShapes();
}

Shape* Game::createBoxes()
{
	//random dropping of boxes
    duration = timer.asSeconds();
//	staticPlatform = ++staticPlatform % 10;
    
	if(duration > secPerDrops * boxCount)
	{
		Shape* newBox;
//		if(staticPlatform == 0)
//		{
//			newBox = lastStaticShape = shapeFactory->createRandomShape(viewOffset, false, 98);
//			staticPlatform = 0;
//		}
//		else
        newBox = shapeFactory->createRandomShape(viewOffset);
        
		boxes.push_back(newBox);
        boxCount++;
        score += 50;
		
//        clock.restart();
		return boxes.back();
	}	
	else
		return nullptr;
}

void Game::addRemotePlayer(Player* p)
{
	remotePlayers.push_back(p);
	p->init();
}

void Game::exitGame()
{
	cout << "Exiting game..." << endl;
	// localHost is not defined when in main menu
	if(localHost != nullptr) {
        localHost->exit = true;
        if(localHost->isServer())
        {
            allowJoin = false; //prepare listen thread for shutdown
            Server* server = dynamic_cast<Server*>(localHost);
            sf::Packet packet;
            packet << UDPNetwork::SERVER_EXIT;
            packet << player->getLocalID();
            server->broadCast(packet);
            cout << "Waiting for network thread to join..." << endl;
        }
        else
        {
            Client* client = dynamic_cast<Client*>(localHost);
            sf::Packet packet;
            packet << UDPNetwork::CLIENT_EXIT;
            packet << player->getLocalID();
            client->sendToServer(packet);
            
        }
        cout << "Waiting for listen thread to join..."<< endl;
        lobby->getNetwork()->join();
    }
	window->close();
	exitCalled = true;
	cout << "Good bye!" << endl;
}

bool Game::removeRemotePlayer(int localID)
{
	//remove remote player
	list<Player*>::iterator i = remove_if(remotePlayers.begin(), remotePlayers.end(), [localID](Player* p)
	{
		return p->getLocalID() == localID;
	});
	if(i != remotePlayers.end())
	{
		delete *i;
		remotePlayers.erase(i);
		if(!localHost->isServer())
			return true;
		else
			return dynamic_cast<Server*>(localHost)->dropPlayer(localID);
	}
	else
		return false;
}

void Game::playerBoxInteraction()
{
	int id;
	b2ContactEdge* edge = player->getBody()->GetContactList();
	for(;edge; edge = edge->next)
	{
		if(edge->contact->IsTouching())
		{
			id = ((b2BodyUserData*)edge->other->GetUserData())->id;

			//id >0 means that it is not ground i.e. blocks of some kind that isn't players
			if(id > 0)
			{
				//Also check if last entered element in localChanges is for the same block so that we 
				//can save data. Overwrite last element if the same
				bool push;
				shapeSync* s;
				if(!localChanges.empty() && localChanges.back()->shapeID == id)
				{
					s = localChanges.back();
					push = false;
				}
				else
				{
					s = new shapeSync;
					push = true;
				}
				s->shapeID = id;
				s->angularVel = edge->other->GetAngularVelocity();
				s->velocity = edge->other->GetLinearVelocity();
				s->position = edge->other->GetPosition();
				s->angle = edge->other->GetAngle();
//				s->collisionID = (uintptr_t)edge->other->GetFixtureList()->GetUserData();
				Shape* shape = getShape(id);
				if(shape != nullptr)
					shape->resetUpdateClock();
				if(push)
					localChanges.push_back(s);
			}
		}
	}
}

void Game::updateShapes(shapeSync* s)
{
	synchronizer->updateShapes(s);
}

void Game::updatePlayer(player_info* p)
{
	synchronizer->updatePlayer(p);
}

void Game::removeShape(int id)
{
	vector<Shape*>::iterator del = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return s->getId() == id;
	});
	if(del != boxes.end())
	{
		(*del)->getUserData().toBeRemoved = true;
		delete *del;
		boxes.erase(del);
	}
	else
		cout << "Shape to be removed not found! ID: "<< id << endl;
}

void Game::playerHandling()
{
	player->draw();
	player->update();
    
	remover->checkPlayerKill(player);
	remover->respawnPlayer(player);

	for(list<Player*>::iterator itr = remotePlayers.begin(); itr != remotePlayers.end(); itr++)
	{
		Player* remotePlayer = *itr;
		remotePlayer->draw();
		remotePlayer->update();
	}	
}

Shape* Game::getShape(int id)
{
	vector<Shape*>::iterator found = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return id == s->getId();
	});
	if(found != boxes.end())
		return *found;
	else
		return nullptr;
}

Player* Game::getRemotePlayer(int localID)
{
	list<Player*>::iterator found = find_if(remotePlayers.begin(), remotePlayers.end(), [localID](Player* player)
	{
		return localID == player->getLocalID();
	});
	if(found != remotePlayers.end())
		return *found;
	else
		return nullptr;
}

Player* Game::getPlayer(int localID)
{
	if(player->getLocalID() == localID)
		return player;
	else
		return getRemotePlayer(localID);
}

void Game::startRise()
{
	rise = true;
	sf::Packet p = packetParser->pack<int>(UDPNetwork::START_RISE, 1);
	if(localHost->isServer())
		dynamic_cast<Server*>(localHost)->broadCast(p);
};
