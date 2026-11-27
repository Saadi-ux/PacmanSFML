#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;

char maze[ROWS][COLS];
int prow = 0, pcol = 0, dir = 0;
int score = 0, lives = 3;

extern int Px;
extern int Py;

bool loadMaze(const char* filename, char maze[ROWS][COLS]);
void drawMaze(RenderWindow& window, int& prow, int& pcol, int dir, char maze[ROWS][COLS]);
void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS]);
void movePlayer(int dir, int& prow, int& pcol, char maze[ROWS][COLS]);
void saveGame();
void initGhosts(char maze[ROWS][COLS]);
void initGhostState();
bool updateGhosts(char maze[ROWS][COLS], int prow, int pcol, int oldPRow, int oldPCol, int& score, int& lives);
void frightenGhosts();

int main()
{
	srand(time(0));

	if (!loadMaze("maze.txt", maze)) return 1;
	initGhosts(maze);
	initGhostState();

	RenderWindow window(VideoMode({ 570, 680 }), "Pac-Man");
	window.setFramerateLimit(60);

	int startRow = -1, startCol = -1;

	while (window.isOpen())
	{
		while (const optional event = window.pollEvent())
		{
			if (event->is<Event::Closed>())
				window.close();
		}

		window.clear(Color::Black);

		drawMaze(window, prow, pcol, dir, maze);

		if (startRow < 0)
		{
			startRow = prow;
			startCol = pcol;
		}

		int oldRow = prow, oldCol = pcol;

		readInput(dir, prow, pcol, maze);
		movePlayer(dir, prow, pcol, maze);

		if (updateGhosts(maze, prow, pcol, oldRow, oldCol, score, lives))
		{
			prow = startRow;
			pcol = startCol;
			Px = pcol * TILE + TILE / 2;
			Py = prow * TILE + TILE / 2;
			dir = 0;

			if (lives <= 0)
				window.close();
		}

		window.display();
	}

	return 0;
}