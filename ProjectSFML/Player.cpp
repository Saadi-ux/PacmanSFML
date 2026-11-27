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
void movePlayer(int dir,int& prow,int& pcol,char maze[ROWS][COLS])
{
	if (CTime != FrameTime){
		CTime++;
		return;
	}
	CTime = 1;
	if (dir == 0)
		return;
	if(dir==1 && isWall(maze, prow-1, pcol)!=true)
	{
		maze[prow][pcol] = '-';
		prow--;
	}
	if (dir == 2 && isWall(maze, prow + 1, pcol)!=true)
	{
		maze[prow][pcol] = '-';
		prow++;
	}
	if (dir == 3 && isWall(maze, prow, pcol - 1)!=true)
	{
		maze[prow][pcol] = '-';
		pcol--;
	}
	if (dir == 4 && isWall(maze, prow, pcol + 1)!=true)
	{
		maze[prow][pcol] = '-';
		pcol++;
	}
	maze[prow][pcol] = '-';
}

void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS])
{
	if ((Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) && !isWall(maze, prow - 1, pcol))
		dir = 1;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) && !isWall(maze, prow + 1, pcol))
		dir = 2;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)) && !isWall(maze, prow, pcol - 1))
		dir = 3;
	else if ((Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)) && !isWall(maze, prow, pcol + 1))
		dir = 4;
}