#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
const int ROWS = 40;
const int COLS = 40;
const int FrameTime = 8;
int CTime = 1;
void movePlayer(int dir,int& prow,int& pcol,char maze[ROWS][COLS])
{
	if (CTime != FrameTime){
		CTime++;
		return;
	}
	CTime = 1;
	if (dir == 0)
		return;
	if(dir==1 && maze[prow-1][pcol]!='#')
	{
		maze[prow][pcol] = '-';
		prow--;
		maze[prow][pcol] = 'P';
	}
	if (dir == 2 && maze[prow + 1][pcol] != '#')
	{
		maze[prow][pcol] = '-';
		prow++;
		maze[prow][pcol] = 'P';
	}
	if (dir == 3 && maze[prow][pcol - 1] != '#')
	{
		maze[prow][pcol] = '-';
		pcol--;
		maze[prow][pcol] = 'P';
	}
	if (dir == 4 && maze[prow][pcol + 1] != '#')
	{
		maze[prow][pcol] = '-';
		pcol++;
		maze[prow][pcol] = 'P';
	}

}
void readInput(int& dir)
{
	if (Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up))
		dir = 1;

	else if (Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down))
		dir = 2;

	else if (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left))
		dir = 3;

	else if (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right))
		dir = 4;
}