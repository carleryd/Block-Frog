
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"
#include "Game.h"
#include "Controller.h"
#include <Box2D/Box2D.h>

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML window");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a sprite to display
//    sf::Texture texture;
//    if (!texture.loadFromFile(resourcePath() + "cute_image.jpg")) {
//        return EXIT_FAILURE;
//    }
//    sf::Sprite sprite(texture);

    // Create a graphical text to display
//    sf::Font font;
//    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
//        return EXIT_FAILURE;
//    }
//    sf::Text text("Hello SFML", font, 50);
//    text.setColor(sf::Color::Black);

    // Load a music to play
//    sf::Music music;
//    if (!music.openFromFile(resourcePath() + "nice_music.ogg")) {
//        return EXIT_FAILURE;
//    }

    // Play the music
//    music.play();
    
    Game* game = new Game(window);
    Controller* controller = new Controller();

    // Start the game loop
    while (window->isOpen())
    {
        // Clear screen
        window->clear();
        
        game->run();
        
        controller->checkInput(game);
        
        // New comment!

        // Update the window
        window->display();
    }

    return EXIT_SUCCESS;
}
