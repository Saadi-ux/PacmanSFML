
#include <iostream>
#include<fstream>

using namespace std;

const int ROWS = 40;
const int COLS = 40;
int rows, cols;
void saveGame(char maze[ROWS][COLS])
{
	ifstream file("maze.txt");
	file >> rows >> cols;
	file.close();

	ofstream file1("maze.txt");
	file1 << rows << " " << cols << endl;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			file1 << maze[i][j] << " ";
		}
		file1 << endl;
	}

	file.close();
}
void findPlayer(int& prow, int& pcol, char maze[ROWS][COLS])
{
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] == 'P')
			{
				prow = r;
				pcol = c;
				return;
			}
		}

	}
}