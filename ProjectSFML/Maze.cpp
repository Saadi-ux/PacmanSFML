#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
const int NUM_GHOSTS = 4;
const float HUD_H = 50.f;
bool showBlue(int i);

int Px;
int Py;
int ghostRow[NUM_GHOSTS];
int ghostCol[NUM_GHOSTS];
int ghostDir[NUM_GHOSTS];
int ghostStartRow[NUM_GHOSTS];
int ghostStartCol[NUM_GHOSTS];

void findPlayer(int& prow, int& pcol, char maze[ROWS][COLS]);
void drawGhost(RenderWindow& window, float cx, float cy, Color color, bool scared, int facing);

bool loadMaze(const char* filename, char maze[ROWS][COLS])
{
	ifstream file(filename);
	if (!file)
	{
		cout << "Could not open " << filename << endl;
		return false;
	}

	int rows, cols;
	file >> rows >> cols;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			file >> maze[i][j];

	file.close();
	return true;
}

bool isWall(char maze[ROWS][COLS], int r, int c);

void drawWalls(RenderWindow& window, char maze[ROWS][COLS]);

void  drawPlayer(RenderWindow& window, int prow, int pcol, int dir);

void drawPellets(RenderWindow& window, char maze[ROWS][COLS]);

void initGhosts(char maze[ROWS][COLS])
{
	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		ghostRow[i] = ghostCol[i] = -1;
		ghostStartRow[i] = ghostStartCol[i] = -1;
		ghostDir[i] = 0;
	}

	int n = 0;
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] == 'G' && n < NUM_GHOSTS)
			{
				ghostStartRow[n] = ghostRow[n] = r;
				ghostStartCol[n] = ghostCol[n] = c;
				maze[r][c] = '-';
				n++;
			}
		}
	}
}


void drawMaze(RenderWindow& window, int& prow, int& pcol, int dir, char maze[ROWS][COLS])
{
	drawWalls(window, maze);
	drawPellets(window, maze);
	findPlayer(prow, pcol, maze);

	const Color ghostColors[NUM_GHOSTS] = {
		Color::Red,
		Color::Cyan,
		Color(255, 192, 203),
		Color(255, 165, 0)
	};

	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		if (ghostRow[i] < 0) continue;
		float gx = ghostCol[i] * TILE + TILE / 2;
		float gy = ghostRow[i] * TILE + HUD_H + TILE / 2;
		drawGhost(window, gx, gy, ghostColors[i], showBlue(i), ghostDir[i]);
	}

	drawPlayer(window, prow, pcol, dir);
}