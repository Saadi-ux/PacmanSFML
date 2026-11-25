#include <iostream>
#include <SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>

using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
char maze[ROWS][COLS];                 // gloabal
int prow=0,pcol=0,dir=0; // Player row and column position
extern char maze[ROWS][COLS];          // defined in Maze.cpp
bool loadMaze(const char* filename,char maze[ROWS][COLS]);   // defined in Maze.cpp
void drawMaze(RenderWindow& window,int &prow,int &pcol,char maze[ROWS][COLS]);   // defined in Maze.cpp
void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS]);                                                      // defined in Player.cpp
void movePlayer(int dir, int& prow, int& pcol, char maze[ROWS][COLS]);
void saveGame();



int main()
{
	if (!loadMaze("maze.txt", maze)) return 1;

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

		drawMaze(window, prow, pcol, maze);

		readInput(dir, prow, pcol, maze);
		movePlayer(dir, prow, pcol, maze);

		window.display();

	}

	return 0;
}