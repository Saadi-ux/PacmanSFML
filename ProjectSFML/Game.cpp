#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

const float HUD_H = 50.f;
const int ROWS = 40;
const int COLS = 40;
const int NUM_GHOSTS = 4;
const char SAVE_FILE[] = "savegame.txt";

int rows, cols;

extern int ghostRow[NUM_GHOSTS];
extern int ghostCol[NUM_GHOSTS];

// Reads the maze size from maze.txt (21 x 19)
void readMazeSize()
{
	ifstream size("maze.txt");
	size >> rows >> cols;
	size.close();
}

// Line 1: score lives | Line 2: player row col | Line 3: ghost rows/cols | then the maze rows
void saveGame(char maze[ROWS][COLS], int score, int lives, int prow, int pcol)
{
	readMazeSize();

	ofstream file(SAVE_FILE);
	file << score << " " << lives << endl;
	file << prow << " " << pcol << endl;
	for (int i = 0; i < NUM_GHOSTS; i++)
		file << ghostRow[i] << " " << ghostCol[i] << " ";
	file << endl;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
			file << maze[i][j];
		file << endl;
	}
	file.close();
}

// Loads savegame.txt. Returns false if there is no save or it cannot be read.
bool loadGame(char maze[ROWS][COLS], int& score, int& lives, int& prow, int& pcol)
{
	readMazeSize();

	ifstream file(SAVE_FILE);
	if (!file)
		return false;

	file >> score >> lives;
	file >> prow >> pcol;
	for (int i = 0; i < NUM_GHOSTS; i++)
		file >> ghostRow[i] >> ghostCol[i];

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			file >> maze[i][j];

	if (!file)
		return false;

	file.close();
	return true;
}

// A save exists only if the file exists AND is not empty
bool hasSave()
{
	ifstream file(SAVE_FILE);
	if (!file)
		return false;
	return file.peek() != ifstream::traits_type::eof();
}

// Opening with ofstream and closing straight away empties the file
void deleteSave()
{
	ofstream file(SAVE_FILE);
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