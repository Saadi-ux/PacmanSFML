#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
const int NUM_GHOSTS = 4;
bool showBlue(int i);

int Px;
int Py;
int ghostRow[NUM_GHOSTS];
int ghostCol[NUM_GHOSTS];
int ghostDir[NUM_GHOSTS];
int ghostStartRow[NUM_GHOSTS];
int ghostStartCol[NUM_GHOSTS];

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

	return true;
}

bool isWall(char maze[ROWS][COLS], int r, int c)
{
	if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return true;
	return maze[r][c] == '#';
}

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

void drawWalls(RenderWindow& window, char maze[ROWS][COLS])
{
	const float T = 3.f;
	RectangleShape fill({ TILE, TILE });
	fill.setFillColor(Color(22, 32, 90));

	RectangleShape horizontal({ TILE + T, T });
	RectangleShape vertical({ T, TILE + T });
	horizontal.setFillColor(Color(66, 110, 255));
	vertical.setFillColor(Color(66, 110, 255));

	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (maze[r][c] == '#')
			{
				fill.setPosition({ c * TILE, r * TILE });
				window.draw(fill);
			}

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] != '#') continue;
			float x = c * TILE;
			float y = r * TILE;

			if (!isWall(maze, r - 1, c)) { horizontal.setPosition({ x - T / 2, y - T / 2 });        window.draw(horizontal); }
			if (!isWall(maze, r + 1, c)) { horizontal.setPosition({ x - T / 2, y + TILE - T / 2 }); window.draw(horizontal); }
			if (!isWall(maze, r, c - 1)) { vertical.setPosition({ x - T / 2, y - T / 2 });          window.draw(vertical); }
			if (!isWall(maze, r, c + 1)) { vertical.setPosition({ x + TILE - T / 2, y - T / 2 });   window.draw(vertical); }
		}
	}
}

void drawMaze(RenderWindow& window, int& prow, int& pcol, char maze[ROWS][COLS])
{
	drawWalls(window, maze);

	CircleShape pellet(2.5);
	pellet.setFillColor(Color::White);

	CircleShape powerPellet(8.f);
	powerPellet.setFillColor(Color::White);

	CircleShape player(9.f);
	player.setFillColor(Color::Yellow);

	CircleShape ghost(9.f);
	const Color ghostColors[NUM_GHOSTS] = {
		Color::Red,
		Color::Cyan,
		Color(255, 192, 203),
		Color(255, 165, 0)
	};

	float Newx, Newy;

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			Newx = c * TILE;
			Newy = r * TILE;

			if (maze[r][c] == '.')
			{
				pellet.setPosition({ Newx + TILE / 2 - 2.5f, Newy + TILE / 2 - 2.5f });
				window.draw(pellet);
			}
			else if (maze[r][c] == 'o')
			{
				powerPellet.setPosition({ Newx + TILE / 2 - 8.f, Newy + TILE / 2 - 8.f });
				window.draw(powerPellet);
			}
			else if (maze[r][c] == 'P')
			{
				Px = c * TILE + TILE / 2;
				Py = r * TILE + TILE / 2;
				maze[r][c] = '-';
				prow = r;
				pcol = c;
			}
		}
	}

	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		if (ghostRow[i] < 0) continue;
		ghost.setFillColor(showBlue(i) ? Color(100, 100, 255) : ghostColors[i]);
		ghost.setPosition({ ghostCol[i] * TILE + TILE / 2 - 9.f,
							ghostRow[i] * TILE + TILE / 2 - 9.f });
		window.draw(ghost);
	}

	player.setPosition({ Px - 9.f, Py - 9.f });
	window.draw(player);
}