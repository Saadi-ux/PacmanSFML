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
void readInput(int& pcol, int& prow,char maze[ROWS][COLS])
{
	if (Keyboard::isKeyPressed(Keyboard::Key::W)||Keyboard::isKeyPressed(Keyboard::Key::Up))
	{
		movePlayer(1, prow,pcol,maze);
	}
	else if (Keyboard::isKeyPressed(Keyboard::Key::S)||Keyboard::isKeyPressed(Keyboard::Key::Down))
	{
		movePlayer(2, prow, pcol,maze);
	}
	else if (Keyboard::isKeyPressed(Keyboard::Key::A)||Keyboard::isKeyPressed(Keyboard::Key::Left))
	{
		movePlayer(3, prow, pcol,maze);
	}
	else if (Keyboard::isKeyPressed(Keyboard::Key::D)||Keyboard::isKeyPressed(Keyboard::Key::Right))
	{
		movePlayer(4, prow, pcol,maze);
	}
}