#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
const int NUM_GHOSTS = 4;

bool isWall(char maze[ROWS][COLS], int r, int c)
{
	if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return true;
	return maze[r][c] == '#';
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


void drawPlayer(RenderWindow& window, int prow, int pcol, int dir)
{
	float cx = pcol * TILE + TILE / 2;
	float cy = prow * TILE + TILE / 2;
	const float R = 9.f;

	// body
	CircleShape body(R);
	body.setFillColor(Color::Yellow);
	body.setOrigin({ R, R });
	body.setPosition({ cx, cy });
	window.draw(body);

	// mouth: a black wedge that opens and closes, facing the direction of travel
	static int frame = 0;                       // counts calls, so one per frame
	frame++;
	float open = 30.f;                          // standing still: half open
	if (dir != 0)
		open = ((frame / 6) % 2 == 0) ? 40.f : 8.f;   // chomp

	float face = 0.f;                           // right
	if (dir == 1) face = 270.f;                 // up
	else if (dir == 2) face = 90.f;             // down
	else if (dir == 3) face = 180.f;            // left

	float h = open * 3.14159265f / 180.f;       // half-angle in radians
	ConvexShape mouth(3);
	mouth.setPoint(0, { 0.f, 0.f });
	mouth.setPoint(1, { 12.f * cos(h), -12.f * sin(h) });
	mouth.setPoint(2, { 12.f * cos(h),  12.f * sin(h) });
	mouth.setFillColor(Color::Black);
	mouth.setPosition({ cx, cy });
	mouth.setRotation(degrees(face));
	window.draw(mouth);

	// ears: drawn after the mouth so they stay visible when facing up
	CircleShape ear(5.f, 3);
	ear.setFillColor(Color::Yellow);
	ear.setOrigin({ 5.f, 5.f });
	CircleShape inner(2.5f, 3);
	inner.setFillColor(Color(255, 150, 170));
	inner.setOrigin({ 2.5f, 2.5f });

	ear.setPosition({ cx - 6.f, cy - 8.f });   ear.setRotation(degrees(-15.f));
	window.draw(ear);
	inner.setPosition({ cx - 6.5f, cy - 10.5f }); inner.setRotation(degrees(-15.f));
	window.draw(inner);

	ear.setPosition({ cx + 6.f, cy - 8.f });   ear.setRotation(degrees(15.f));
	window.draw(ear);
	inner.setPosition({ cx + 6.5f, cy - 10.5f }); inner.setRotation(degrees(15.f));
	window.draw(inner);

}

void drawPellets(RenderWindow& window, char maze[ROWS][COLS])
{
	CircleShape pellet(2.5);
	pellet.setFillColor(Color::White);
	CircleShape powerPellet(8.f);
	powerPellet.setFillColor(Color::White);
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] == '.')
			{
				pellet.setPosition({ c * TILE + TILE / 2 - 2.5f, r * TILE + TILE / 2 - 2.5f });
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

// facing uses your ghost direction: 0 up, 1 left, 2 down, 3 right (-1 = none)

void drawGhost(RenderWindow& window, float cx, float cy, Color color, bool scared, int facing)
{
	Color bodyColor = scared ? Color(100, 100, 255) : color;

	// head (top half is round), body block, and three feet along the bottom
	CircleShape head(10.f);
	head.setFillColor(bodyColor);
	head.setOrigin({ 10.f, 10.f });
	head.setPosition({ cx, cy - 2.f });
	window.draw(head);

	RectangleShape body({ 20.f, 8.f });
	body.setFillColor(bodyColor);
	body.setPosition({ cx - 10.f, cy - 2.f });
	window.draw(body);

	CircleShape foot(3.34f);
	foot.setFillColor(bodyColor);
	foot.setOrigin({ 3.34f, 3.34f });
	for (int k = -1; k <= 1; k++)
	{
		foot.setPosition({ cx + k * 6.67f, cy + 6.f });
		window.draw(foot);
	}

	if (!scared)
	{
		// white eyes with blue pupils that look the way the ghost is moving
		float ox = 0.f, oy = 0.f;
		if (facing == 0) oy = -1.5f;
		else if (facing == 1) ox = -1.5f;
		else if (facing == 2) oy = 1.5f;
		else if (facing == 3) ox = 1.5f;

		CircleShape eye(3.f);
		eye.setFillColor(Color::White);
		eye.setOrigin({ 3.f, 3.f });
		CircleShape pupil(1.6f);
		pupil.setFillColor(Color(33, 33, 222));
		pupil.setOrigin({ 1.6f, 1.6f });

		for (int s = -1; s <= 1; s += 2)
		{
			eye.setPosition({ cx + s * 4.f, cy - 3.f });
			window.draw(eye);
			pupil.setPosition({ cx + s * 4.f + ox, cy - 3.f + oy });
			window.draw(pupil);
		}
	}
	else
	{
		// frightened face: two small dots and a zigzag mouth
		CircleShape dot(1.5f);
		dot.setFillColor(Color::White);
		dot.setOrigin({ 1.5f, 1.5f });
		dot.setPosition({ cx - 3.5f, cy - 3.f });
		window.draw(dot);
		dot.setPosition({ cx + 3.5f, cy - 3.f });
		window.draw(dot);

		RectangleShape seg({ 3.f, 1.5f });
		seg.setFillColor(Color::White);
		for (int k = 0; k < 4; k++)
		{
			seg.setPosition({ cx - 6.f + k * 3.f, cy + 3.f + (k % 2) * 2.f });
			window.draw(seg);
		}
	}
}