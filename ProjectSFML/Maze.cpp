#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;


bool loadMaze(const char* filename,char maze[ROWS][COLS])
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
int Px; 
int Py;
void drawMaze(RenderWindow& window, int &prow, int &pcol,char maze[ROWS][COLS])
{
    RectangleShape wallBlock({ TILE, TILE });
    wallBlock.setFillColor(Color(33, 33, 222));

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
            if (maze[r][c] == '#')
            {
                wallBlock.setPosition({ Newx, Newy });
                window.draw(wallBlock);
            }
            else if (maze[r][c] == '.')
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