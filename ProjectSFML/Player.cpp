#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
const int ROWS = 40;
const int COLS = 40;
const int FrameTime = 8;
int CTime = 1;
bool isWall(char maze[ROWS][COLS], int r, int c);
// Moves the player one tile. Returns the points for the pellet eaten: 10 = dot, 50 = power pellet, 0 = nothing
int movePlayer(int dir, int& prow, int& pcol, char maze[ROWS][COLS])
{
	if (CTime != FrameTime) {
		CTime++;
		return 0;
	}
	CTime = 1;
	if (dir == 0)
		return 0;
	if (dir == 1 && isWall(maze, prow - 1, pcol) != true)
		prow--;

	if (dir == 2 && isWall(maze, prow + 1, pcol) != true)
		prow++;

	if (dir == 3)
	{
		if (pcol == 0)
			pcol = 18;

		else if (isWall(maze, prow, pcol - 1) != true)
			pcol--;

	}
	if (dir == 4)
	{
		if (pcol == 18)
		{
			if (isWall(maze, prow, pcol + 1) != true)
				pcol = 0;
		}
		else if (isWall(maze, prow, pcol + 1) != true)
		{
			pcol++;
		}
	}

	// What is on the tile we ended up on?
	int points = 0;
	if (maze[prow][pcol] == '.')
		points = 10;
	else if (maze[prow][pcol] == 'o')
		points = 50;

	maze[prow][pcol] = '-';       // the pellet is eaten
	return points;
}

void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS])
{
	int leftCol = pcol - 1;
	if (pcol == 0)
		leftCol = 18;
	int rightCol = pcol + 1;
	if (pcol == 18)
		rightCol = 0;
	if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && !isWall(maze, prow - 1, pcol))
		dir = 1;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && !isWall(maze, prow + 1, pcol))
		dir = 2;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)) && !isWall(maze, prow, leftCol))
		dir = 3;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)) && !isWall(maze, prow, rightCol))
		dir = 4;
}