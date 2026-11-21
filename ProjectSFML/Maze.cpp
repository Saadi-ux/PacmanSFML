#include <iostream>
#include <fstream>
using namespace std;
void loadMaze()
{
	char Maze[50][50];
	int rows, cols;
	ifstream file("maze.txt");
	file >> rows;
	file >> cols;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			file >> Maze[i][j];


}

void drawMaze()
{
	//  This is Drawing code
}