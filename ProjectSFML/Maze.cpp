#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
int Px;
int Py;

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
} // Anything outside the maze counts as wall, so the window edge gets no line

void drawWalls(RenderWindow& window, char maze[ROWS][COLS])
{
	const float T = 3.f;                          // border thickness
	RectangleShape fill({ TILE, TILE });
	fill.setFillColor(Color(22, 32, 90));

	RectangleShape horizontal({ TILE + T, T });   // T longer so corners join
	RectangleShape vertical({ T, TILE + T });
	horizontal.setFillColor(Color(66, 110, 255));
	vertical.setFillColor(Color(66, 110, 255));

	// fills first, so they never cover a border

	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (maze[r][c] == '#')
			{
				fill.setPosition({ c * TILE, r * TILE });
				window.draw(fill);
			}

	// border only on sides that face a non-wall
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

	drawWalls(window,maze);

	CircleShape pellet(3.f);
	pellet.setFillColor(Color::White);

	CircleShape powerPellet(8.f);
	powerPellet.setFillColor(Color::Yellow);

	CircleShape player(9.f);
	player.setFillColor(Color::Blue);

	float Newx, Newy;

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			Newx = c * TILE;
			Newy = r * TILE;

			if (maze[r][c] == '.')
			{
				pellet.setPosition({ Newx + TILE / 2 - 3.f, Newy + TILE / 2 - 3.f });
				window.draw(pellet);
			}
			else if (maze[r][c] == 'o')
			{
				powerPellet.setPosition({ Newx + TILE / 2 - 8.f, Newy + TILE / 2 - 8.f });
				window.draw(powerPellet);
			}
			if (maze[r][c] == 'P')
			{
				Px = c * TILE + TILE / 2;
				Py = r * TILE + TILE / 2;
				maze[r][c] = '-'; // Clear the initialplayer position drawing
				prow = r;
				pcol = c;
			}

		}

	}
	player.setPosition({ Px - 10.f, Py - 10.f });
	window.draw(player);
}