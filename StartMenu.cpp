#include "StartMenu.h"
#include "ShapeFactory.h"
#include "Game.h"
#include "Player.h"
#include "Textor.h"
#include "OSHandler.h"
#include "Utility.h"

StartMenu::StartMenu(Game* game_) {
    game = game_;
    game->setLocalID(1);
    world = game->getWorld();
    shapeFactory = game->getShapeFactory();
    window = game->getWindow();
    player = new Player(game, 1, "green");
    player->setPosition(new b2Vec2(0, 0));
    textor = game->getTextor();
    osHandler = game->getOSHandler();
    utility = game->getUtility();
    
    player->init();
    game->setPlayer(player);
    
    hostRectangle = shapeFactory->createRectangle(new b2Vec2(100.0f, 50.0f),
                                                  new b2Vec2(-200.0f, -50.0f),
                                                  false,
                                                  1.0,
                                                  1.0);
    hostRectangle->getBody()->GetFixtureList()->SetUserData((void*)80);
    
    joinRectangle = shapeFactory->createRectangle(new b2Vec2(100.0f, 50.0f),
                                                  new b2Vec2(200.0f, -50.0f),
                                                  false ,
                                                  1.0,
                                                  1.0);
    joinRectangle->getBody()->GetFixtureList()->SetUserData((void*)81);
    
    floor = shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f),
                                          new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                          false,
                                          1.0,
                                          1.0);
    
    if(!textureA.loadFromFile(osHandler->getResourcePath() + "suggested_A.png")) {
        cout << "Could not load A image" << endl;
    }
    if(!textureD.loadFromFile(osHandler->getResourcePath() + "suggested_D.png")) {
        cout << "Could not load D image" << endl;
    }
    if(!textureSpace.loadFromFile(osHandler->getResourcePath() + "suggested_space.png")) {
        cout << "Could not load space image" << endl;
    }
    if(!textureLeftClick.loadFromFile(osHandler->getResourcePath() + "suggested_leftclick.png")) {
        cout << "Could not load space image" << endl;
    }
    
    spriteA.setTexture(textureA);
    spriteD.setTexture(textureD);
    spriteSpace.setTexture(textureSpace);
    spriteLeftClick.setTexture(textureLeftClick);
    
    spriteA.setScale(0.4, 0.4);
    spriteD.setScale(0.4, 0.4);
    spriteSpace.setScale(0.4, 0.4);
    spriteLeftClick.setScale(0.4, 0.4);
    
    APressed = false;
    DPressed = false;
    spacePressed = false;
    leftClickPressed = false;
    
    clock.restart();
    learnState = WALK;
    
    window->setActive(true);
}

void StartMenu::run() {
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 8);
    world->ClearForces();
    
    timer = clock.getElapsedTime();
    
    player->draw();
	player->update();
    
    if((!APressed || !DPressed) && timer.asSeconds() > 2) {
        learnState = WALK;
    }
    else if(learnState == WALK) {
        clock.restart();
        learnState = WAITING;
    }
    else if(!spacePressed && timer.asSeconds() > 2) {
        learnState = JUMP;
    }
    else if(learnState == JUMP) {
        clock.restart();
        learnState = WAITING;
    }
    else if(!leftClickPressed && timer.asSeconds() > 2) {
        learnState = HOOK;
    }
    
    playerPos = utility->B2DtoSFML(game->getPlayer()->getBody()->GetPosition());
    
    switch(learnState)
    {
        case WALK:
        {
            if(!APressed) {
                window->draw(spriteA);
                spriteA.setPosition(sf::Vector2f(playerPos.x - 50 - textureA.getSize().x/2 * spriteA.getScale().x,
                                                 playerPos.y - textureA.getSize().y/2 * spriteA.getScale().y));
            }
            if(!DPressed) {
                window->draw(spriteD);
                spriteD.setPosition(sf::Vector2f(playerPos.x + 50 - textureD.getSize().x/2 * spriteD.getScale().x,
                                                 playerPos.y - textureD.getSize().y/2 * spriteD.getScale().y));
            }
        }
            break;
        case JUMP:
        {
            if(!spacePressed) {
	            window->draw(spriteSpace);
    	        spriteSpace.setPosition(sf::Vector2f(playerPos.x - textureSpace.getSize().x/2
                                                     * spriteSpace.getScale().x,
                                                 playerPos.y - 50 - textureSpace.getSize().y/2
                                                     * spriteSpace.getScale().y));
            }
        }
            break;
        case HOOK:
        {
            if(!leftClickPressed) {
                window->draw(spriteLeftClick);
    	        spriteLeftClick.setPosition(sf::Vector2f(playerPos.x - textureLeftClick.getSize().x/2
                                                     * spriteLeftClick.getScale().x,
                                                     playerPos.y - 50 - textureLeftClick.getSize().y/2
                                                     * spriteLeftClick.getScale().y));
            }
        }
            break;
        case WAITING:
        {
            
        }
            break;
        default: ;
    }
    
    hostRectangle->update();
    window->draw(*hostRectangle->getShape());
    joinRectangle->update();
    window->draw(*joinRectangle->getShape());
    floor->update();
    window->draw(*floor->getShape());
	
    window->draw(textor->write("Make a choice", sf::Vector2f(window->getSize().x/2, 100)));
	window->draw(textor->write("Host", hostRectangle->getShape()->getPosition() - sf::Vector2f(0, 10)));
    window->draw(textor->write("Join", joinRectangle->getShape()->getPosition() - sf::Vector2f(0, 10)));
}

StartMenu::~StartMenu() {
    delete hostRectangle;
    hostRectangle = nullptr;
    delete joinRectangle;
    joinRectangle = nullptr;
    delete floor;
    floor = nullptr;
    
    game->setPlayer(nullptr);
	delete player;
    player = nullptr;
}