#include <cstdlib>
using namespace std;

const int ROWS = 40;
const int COLS = 40;
const int NUM_GHOSTS = 4;

const int NO_DIR = -1;
const int NORM_SPD = 10;
const int BLUE_SPD = 16;
const int BLUE_TIME = 5 * 60;
const int GHOST_PTS = 200;

// up, left, down, right
const int dr[4] = { -1, 0, 1, 0 };
const int dc[4] = { 0, -1, 0, 1 };

extern int ghostRow[NUM_GHOSTS];
extern int ghostCol[NUM_GHOSTS];
extern int ghostDir[NUM_GHOSTS];
extern int ghostStartRow[NUM_GHOSTS];
extern int ghostStartCol[NUM_GHOSTS];

bool isWall(char maze[ROWS][COLS], int r, int c);

bool ghostBlue[NUM_GHOSTS];
int ghostTick[NUM_GHOSTS];
int blueLeft = 0;
int lastPower = -1;

int countPower(char maze[ROWS][COLS])
{
	int n = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (maze[r][c] == 'o')
				n++;
	return n;
}

bool isChaser(int i)
{
	return i == 0 || i == 2;
}

void sendHome(int i)
{
	ghostRow[i] = ghostStartRow[i];
	ghostCol[i] = ghostStartCol[i];
	ghostDir[i] = NO_DIR;
	ghostBlue[i] = false;
	ghostTick[i] = 0;
}

void initGhostState()
{
	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		ghostDir[i] = NO_DIR;
		ghostBlue[i] = false;
		ghostTick[i] = 0;
	}
	blueLeft = 0;
	lastPower = -1;
}

void resetAll()
{
	for (int i = 0; i < NUM_GHOSTS; i++)
		if (ghostStartRow[i] >= 0)
			sendHome(i);
	blueLeft = 0;
}

void frightenGhosts()
{
	for (int i = 0; i < NUM_GHOSTS; i++)
		if (ghostRow[i] >= 0)
			ghostBlue[i] = true;
	blueLeft = BLUE_TIME;
}

// true while the ghost is frightened (does not flash, unlike showBlue)
bool isBlue(int i)
{
	return ghostBlue[i];
}

bool showBlue(int i)
{
	if (!ghostBlue[i]) return false;
	if (blueLeft > 120) return true;

	// last 2 seconds: flip between blue and normal every 10 framesa
	return (blueLeft / 10) % 2 == 1;
}

int pickDir(int i, char maze[ROWS][COLS], int pr, int pc)
{
	int opts[4];
	int n = 0;

	int back = -1;
	if (ghostDir[i] != NO_DIR)
		back = (ghostDir[i] + 2) % 4;

	// open cells, not counting the way it came from
	for (int d = 0; d < 4; d++)
	{
		if (d == back) continue;
		if (!isWall(maze, ghostRow[i] + dr[d], ghostCol[i] + dc[d]))
			opts[n++] = d;
	}

	// dead end, only choice is to turn around
	if (n == 0)
	{
		if (back != -1 && !isWall(maze, ghostRow[i] + dr[back], ghostCol[i] + dc[back]))
			return back;
		return NO_DIR;
	}

	if (isChaser(i) && !ghostBlue[i])
	{
		int best = opts[0];
		int bestDist = 9999;

		// opts is already in up, left, down, right order so ties keep the first one
		for (int k = 0; k < n; k++)
		{
			int d = opts[k];
			int dist = abs(ghostRow[i] + dr[d] - pr) + abs(ghostCol[i] + dc[d] - pc);
			if (dist < bestDist)
			{
				bestDist = dist;
				best = d;
			}
		}
		return best;
	}

	return opts[rand() % n];
}

// returns true if the player lost a life
bool hit(int i, int& score, int& lives)
{
	if (ghostBlue[i])
	{
		score += GHOST_PTS;
		sendHome(i);
		return false;
	}

	lives--;
	resetAll();
	return true;
}

bool updateGhosts(char maze[ROWS][COLS], int pr, int pc, int oldPr, int oldPc, int& score, int& lives)
{
	if (blueLeft > 0)
	{
		blueLeft--;
		if (blueLeft == 0)
			for (int i = 0; i < NUM_GHOSTS; i++)
				ghostBlue[i] = false;
	}

	// one less power pellet on the map than last frame = the player ate it
	int power = countPower(maze);
	if (lastPower != -1 && power < lastPower)
		frightenGhosts();
	lastPower = power;

	// player walked into a ghost
	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		if (ghostRow[i] < 0) continue;
		if (ghostRow[i] == pr && ghostCol[i] == pc)
			if (hit(i, score, lives))
				return true;
	}

	for (int i = 0; i < NUM_GHOSTS; i++)
	{
		if (ghostRow[i] < 0) continue;

		ghostTick[i]++;
		int spd = ghostBlue[i] ? BLUE_SPD : NORM_SPD;
		if (ghostTick[i] < spd) continue;
		ghostTick[i] = 0;

		int d = pickDir(i, maze, pr, pc);
		if (d == NO_DIR) continue;

		int fromR = ghostRow[i];
		int fromC = ghostCol[i];
		ghostDir[i] = d;
		ghostRow[i] += dr[d];
		ghostCol[i] += dc[d];

		// same cell, or they swapped places
		bool same = (ghostRow[i] == pr && ghostCol[i] == pc);
		bool swap = (fromR == pr && fromC == pc && ghostRow[i] == oldPr && ghostCol[i] == oldPc);

		if (same || swap)
			if (hit(i, score, lives))
				return true;
	}

	return false;
}