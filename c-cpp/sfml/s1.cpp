#include <SFML/Graphics.hpp>
#include <iostream>

int main(int, char const**)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setFramerateLimit(60);
    sf::Clock clock;
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        window.display();
        
        std::cout << (int) (1.0 / clock.restart().asSeconds()) << std::endl;
    }

    return EXIT_SUCCESS;
}