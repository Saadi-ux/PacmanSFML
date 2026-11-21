#include<iostream>
#include<fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

void loadMaze();
void drawMaze();
void movePlayer();
void checkCollisions();
void saveGame();

int main()
{
    RenderWindow window(VideoMode({ 570, 680 }), "Pac-Man");
    window.setFramerateLimit(60); //60fps lock

    const float TILE = 30.f; // Tile size
    
    RectangleShape wallBlock({ TILE, TILE }); // Wall block
    wallBlock.setFillColor(Color(33, 33, 222));
    
    CircleShape pellet(3.f); // Pellet
    pellet.setFillColor(Color::White);


    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }
       
        window.clear(Color::Black);  // Clear the window

       
        window.display();  // Display everything


    }

    return 0;
}
