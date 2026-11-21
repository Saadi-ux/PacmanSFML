#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 21;
const int COLS = 19;
const float TILE = 30.f;

char maze[ROWS][COLS];                 // gloabal

bool loadMaze(const char* filename)
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

void drawMaze(RenderWindow& window)
{
    RectangleShape wallBlock({ TILE, TILE });
    wallBlock.setFillColor(Color(33, 33, 222));

    CircleShape pellet(3.f);
    pellet.setFillColor(Color::White);

    CircleShape powerPellet(8.f);
    powerPellet.setFillColor(Color::Yellow);

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (maze[r][c] == '#')
            {
                wallBlock.setPosition({ c * TILE, r * TILE });
                window.draw(wallBlock);
            }
            else if (maze[r][c] == '.')
            {
                pellet.setPosition({ c * TILE + TILE / 2 - 3.f, r * TILE + TILE / 2 - 3.f });
                window.draw(pellet);
            }
            else if (maze[r][c] == 'o')
            {
                powerPellet.setPosition({ c * TILE + TILE / 2 - 8.f, r * TILE + TILE / 2 - 8.f });
                window.draw(powerPellet);
            }
        }
    }
}