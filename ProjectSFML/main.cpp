#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 21;
const int COLS = 19;
const float TILE = 30.f;

extern char maze[ROWS][COLS];          // defined in Maze.cpp
bool loadMaze(const char* filename);   // defined in Maze.cpp
void drawMaze(RenderWindow& window);   // defined in Maze.cpp

void movePlayer();
void checkCollisions();
void saveGame();

int main()
{
    if (!loadMaze("maze.txt")) return 1;    

    RenderWindow window(VideoMode({ 570, 680 }), "Pac-Man");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (const optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
        }

        window.clear(Color::Black);
        drawMaze(window);                   
        window.display();
    }

    return 0;
}