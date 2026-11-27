#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
const float HUD_H = 50.f;          // height of the HUD strip at the top
const int NUM_GHOSTS = 4;

int catDrawX = 0, catDrawY = 0;      // where the cat is drawn right now (set in drawPlayer)
const int NORM_SPD = 10;           // same values as in Ghost.cpp
const int BLUE_SPD = 16;
const int SLIDE_STEP = 4;          // pixels the cat moves every frame (the cat steps every 8 frames, 4 x 8 covers a tile)

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
				fill.setPosition({ c * TILE, r * TILE + HUD_H });
				window.draw(fill);
			}

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] != '#') continue;
			float x = c * TILE;
			float y = r * TILE + HUD_H;

			if (!isWall(maze, r - 1, c)) { horizontal.setPosition({ x - T / 2, y - T / 2 });        window.draw(horizontal); }
			if (!isWall(maze, r + 1, c)) { horizontal.setPosition({ x - T / 2, y + TILE - T / 2 }); window.draw(horizontal); }
			if (!isWall(maze, r, c - 1)) { vertical.setPosition({ x - T / 2, y - T / 2 });          window.draw(vertical); }
			if (!isWall(maze, r, c + 1)) { vertical.setPosition({ x + TILE - T / 2, y - T / 2 });   window.draw(vertical); }
		}
	}
}

// Visual smoothing only: the game logic still moves whole cells.
// Moves the drawn position (in pixels) toward the target, at most "step" pixels per call.
// moveNow says whether to move on this call. A target that is far away (first frame,
// respawn, ghost sent home) is jumped to instead of slid to.
void slideToward(int& drawX, int& drawY, int targetX, int targetY, bool moveNow, int step)
{
	int dx = targetX - drawX;            // how far the picture still has to go
	int dy = targetY - drawY;
	if (dx > 45 || dx < -45 || dy > 45 || dy < -45)
	{
		drawX = targetX;
		drawY = targetY;
		return;
	}
	if (!moveNow)
		return;
	if (dx > step) dx = step;
	if (dx < -step) dx = -step;
	if (dy > step) dy = step;
	if (dy < -step) dy = -step;
	drawX += dx;
	drawY += dy;
}

void drawPlayer(RenderWindow& window, int prow, int pcol, int dir)
{
	slideToward(catDrawX, catDrawY, (int)(pcol * TILE + TILE / 2), (int)(prow * TILE + HUD_H + TILE / 2), true, SLIDE_STEP);
	float cx = (float)catDrawX;
	float cy = (float)catDrawY;

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

void drawCherry(RenderWindow& window, float cx, float cy)
{
	// stems: two thin rectangles that meet at the top
	RectangleShape stem({ 9.5f, 1.5f });
	stem.setFillColor(Color(60, 140, 40));
	stem.setOrigin({ 0.f, 0.75f });
	stem.setPosition({ cx - 4.f, cy + 0.f });
	stem.setRotation(degrees(-58.f));
	window.draw(stem);
	stem.setPosition({ cx + 4.5f, cy + 1.f });
	stem.setRotation(degrees(-111.f));
	window.draw(stem);

	// leaf at the top where the stems meet
	CircleShape leaf(3.5f, 3);
	leaf.setFillColor(Color(70, 190, 60));
	leaf.setOrigin({ 3.5f, 3.5f });
	leaf.setPosition({ cx + 4.f, cy - 8.f });
	leaf.setRotation(degrees(70.f));
	window.draw(leaf);

	// two red cherries with a small white shine
	CircleShape cherry(4.5f);
	cherry.setFillColor(Color(220, 20, 40));
	cherry.setOrigin({ 4.5f, 4.5f });
	CircleShape shine(1.2f);
	shine.setFillColor(Color::White);
	shine.setOrigin({ 1.2f, 1.2f });

	cherry.setPosition({ cx - 4.5f, cy + 4.f });
	window.draw(cherry);
	shine.setPosition({ cx - 6.f, cy + 2.5f });
	window.draw(shine);

	cherry.setPosition({ cx + 4.5f, cy + 5.f });
	window.draw(cherry);
	shine.setPosition({ cx + 3.f, cy + 3.5f });
	window.draw(shine);
}

void drawPellets(RenderWindow& window, char maze[ROWS][COLS])
{
	// The logic eats a pellet the moment the player arrives on its cell, but the cat is drawn
	// sliding a little behind. So an eaten pellet stays drawn until the cat visually reaches it.
	static char previous[ROWS][COLS];        // the maze as it was last frame
	static int lingerRow = -1, lingerCol = -1;
	static char lingerChar = '-';
	static int lingerFrames = 0;

	CircleShape pellet(2.5);
	pellet.setFillColor(Color::White);
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if ((previous[r][c] == '.' || previous[r][c] == 'o') && maze[r][c] == '-')
			{
				lingerRow = r;               // this pellet was just eaten
				lingerCol = c;
				lingerChar = previous[r][c];
				lingerFrames = 0;
			}
			previous[r][c] = maze[r][c];

			float cx = c * TILE + TILE / 2;
			float cy = r * TILE + HUD_H + TILE / 2;
			if (maze[r][c] == '.')
			{
				pellet.setPosition({ cx - 2.5f, cy - 2.5f });
				window.draw(pellet);
			}
			else if (maze[r][c] == 'o')
			{
				drawCherry(window, cx, cy);
			}
		}
	}

	if (lingerRow >= 0)
	{
		float cx = lingerCol * TILE + TILE / 2;
		float cy = lingerRow * TILE + HUD_H + TILE / 2;
		float gapX = cx - catDrawX;          // how far the drawn cat still is from that cell
		float gapY = cy - catDrawY;
		lingerFrames++;
		if (lingerFrames > 12 || (gapX < 8 && gapX > -8 && gapY < 8 && gapY > -8))
			lingerRow = -1;                  // the cat got there (or it took too long, e.g. a respawn)
		else if (lingerChar == 'o')
			drawCherry(window, cx, cy);
		else
		{
			pellet.setPosition({ cx - 2.5f, cy - 2.5f });
			window.draw(pellet);
		}
	}
}

void drawGhost(RenderWindow& window, int i, float targetCx, float targetCy, Color color, bool scared, bool blue, int facing)
{
	// Visual smoothing only: slide a little every frame, one tile per logic step.
	static float drawX[NUM_GHOSTS], drawY[NUM_GHOSTS];

	float step = TILE / NORM_SPD;
	if (blue)                                // really frightened, even while flashing
		step = TILE / BLUE_SPD;

	float dx = targetCx - drawX[i];
	float dy = targetCy - drawY[i];
	if (fabs(dx) > 45 || fabs(dy) > 45)      // first frame, respawn, sent home: jump
	{
		drawX[i] = targetCx;
		drawY[i] = targetCy;
	}
	else
	{
		if (dx > step) dx = step;
		if (dx < -step) dx = -step;
		if (dy > step) dy = step;
		if (dy < -step) dy = -step;
		drawX[i] += dx;
		drawY[i] += dy;
	}
	float cx = drawX[i];
	float cy = drawY[i];

	Color bodyColor = color;
	if (scared)
		bodyColor = Color(100, 100, 255);

	// head, body block and three feet
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

	// eyes (pupils look the way the ghost moves) or frightened dots
	CircleShape eye(3.f);
	eye.setFillColor(Color::White);
	eye.setOrigin({ 3.f, 3.f });
	CircleShape pupil(1.6f);
	pupil.setFillColor(Color(33, 33, 222));
	pupil.setOrigin({ 1.6f, 1.6f });

	float ox = 0.f, oy = 0.f;
	if (facing == 0) oy = -1.5f;
	else if (facing == 1) ox = -1.5f;
	else if (facing == 2) oy = 1.5f;
	else if (facing == 3) ox = 1.5f;

	for (int s = -1; s <= 1; s += 2)
	{
		if (!scared)
		{
			eye.setPosition({ cx + s * 4.f, cy - 3.f });
			window.draw(eye);
			pupil.setPosition({ cx + s * 4.f + ox, cy - 3.f + oy });
			window.draw(pupil);
		}
		else
		{
			eye.setRadius(1.5f);
			eye.setOrigin({ 1.5f, 1.5f });
			eye.setPosition({ cx + s * 3.5f, cy - 3.f });
			window.draw(eye);
		}
	}

	// frightened zigzag mouth
	if (scared)
	{
		RectangleShape seg({ 3.f, 1.5f });
		seg.setFillColor(Color::White);
		for (int k = 0; k < 4; k++)
		{
			seg.setPosition({ cx - 6.f + k * 3.f, cy + 3.f + (k % 2) * 2.f });
			window.draw(seg);
		}
	}
}

// HUD strip at the top: score on the left, one yellow circle per life on the right
void drawHUD(RenderWindow& window, const Font& font, int score, int lives)
{
	RectangleShape strip({ 570.f, HUD_H });
	strip.setFillColor(Color(10, 10, 30));
	window.draw(strip);

	Text scoreText(font, "Score: " + to_string(score), 24);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition({ 15.f, 10.f });
	window.draw(scoreText);

	CircleShape life(10.f);
	life.setFillColor(Color::Yellow);
	for (int i = 0; i < lives; i++)
	{
		life.setPosition({ 570.f - 40.f - i * 28.f, 15.f });
		window.draw(life);
	}
}