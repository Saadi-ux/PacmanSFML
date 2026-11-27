const int ROWS = 40;
const int COLS = 40;

const int dr[4] = { -1, 0, 1, 0 };
const int dc[4] = { 0, -1, 0, 1 };

extern int ghostRow[4];
extern int ghostCol[4];
extern int ghostDir[4];
extern int ghostStartRow[4];
extern int ghostStartCol[4];

bool isWall(char maze[ROWS][COLS], int r, int c);

bool ghostBlue[4];
int ghostTick[4];
int blueLeft = 0;
int lastPower = -1;
int seed = 12345;

int absValue(int x)
{
	if (x < 0)
		x = -x;
	return x;
}

int randomNumber(int max)
{
	seed = (seed * 75) % 65537;
	return seed % max;
}

int countPower(char maze[ROWS][COLS])
{
	int count = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (maze[r][c] == 'o')
				count++;
	return count;
}

bool isChaser(int i)
{
	if (i == 0 || i == 2)
		return true;
	return false;
}

void sendHome(int i)
{
	ghostRow[i] = ghostStartRow[i];
	ghostCol[i] = ghostStartCol[i];
	ghostDir[i] = -1;
	ghostBlue[i] = false;
	ghostTick[i] = 0;
}

void initGhostState()
{
	for (int i = 0; i < 4; i++)
	{
		ghostDir[i] = -1;
		ghostBlue[i] = false;
		ghostTick[i] = 0;
	}
	blueLeft = 0;
	lastPower = -1;
}

void resetAll()
{
	for (int i = 0; i < 4; i++)
		if (ghostStartRow[i] >= 0)
			sendHome(i);
	blueLeft = 0;
}

void frightenGhosts()
{
	for (int i = 0; i < 4; i++)
		if (ghostRow[i] >= 0)
			ghostBlue[i] = true;
	blueLeft = 300;
}

bool isBlue(int i)
{
	return ghostBlue[i];
}

bool showBlue(int i)
{
	if (ghostBlue[i] == false)
		return false;
	if (blueLeft > 120)
		return true;
	if ((blueLeft / 10) % 2 == 1)
		return true;
	return false;
}

int pickDir(int i, char maze[ROWS][COLS], int prow, int pcol)
{
	int options[4];
	int count = 0;
	int back = -1;
	if (ghostDir[i] != -1)
		back = (ghostDir[i] + 2) % 4;

	if ((ghostCol[i] == 0 && ghostDir[i] == 1) || (ghostCol[i] == 18 && ghostDir[i] == 3))
	{
		if (isWall(maze, ghostRow[i] + dr[back], ghostCol[i] + dc[back]) != true)
			return back;
		return -1;
	}

	for (int d = 0; d < 4; d++)
	{
		if (d != back)
		{
			if (isWall(maze, ghostRow[i] + dr[d], ghostCol[i] + dc[d]) != true)
			{
				options[count] = d;
				count++;
			}
		}
	}

	if (count == 0)
	{
		if (back != -1)
		{
			if (isWall(maze, ghostRow[i] + dr[back], ghostCol[i] + dc[back]) != true)
				return back;
		}
		return -1;
	}

	if (isChaser(i) == true && ghostBlue[i] == false)
	{
		int bestDir = options[0];
		int bestDistance = 9999;
		for (int k = 0; k < count; k++)
		{
			int d = options[k];
			int distance = absValue(ghostRow[i] + dr[d] - prow) + absValue(ghostCol[i] + dc[d] - pcol);
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestDir = d;
			}
		}
		return bestDir;
	}

	return options[randomNumber(count)];
}

bool hit(int i, int& score, int& lives)
{
	if (ghostBlue[i] == true)
	{
		score = score + 200;
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
			for (int i = 0; i < 4; i++)
				ghostBlue[i] = false;
	}

	int power = countPower(maze);
	if (lastPower != -1 && power < lastPower)
		frightenGhosts();
	lastPower = power;

	for (int i = 0; i < 4; i++)
	{
		if (ghostRow[i] >= 0 && ghostRow[i] == pr && ghostCol[i] == pc)
		{
			if (hit(i, score, lives) == true)
				return true;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (ghostRow[i] < 0)
			continue;

		ghostTick[i]++;
		int speed = 10;
		if (ghostBlue[i] == true)
			speed = 16;
		if (ghostTick[i] < speed)
			continue;
		ghostTick[i] = 0;

		int d = pickDir(i, maze, pr, pc);
		if (d == -1)
			continue;

		int fromRow = ghostRow[i];
		int fromCol = ghostCol[i];
		ghostDir[i] = d;
		ghostRow[i] = ghostRow[i] + dr[d];
		ghostCol[i] = ghostCol[i] + dc[d];

		bool sameCell = false;
		if (ghostRow[i] == pr && ghostCol[i] == pc)
			sameCell = true;

		bool swapped = false;
		if (fromRow == pr && fromCol == pc && ghostRow[i] == oldPr && ghostCol[i] == oldPc)
			swapped = true;

		if (sameCell == true || swapped == true)
		{
			if (hit(i, score, lives) == true)
				return true;
		}
	}

	return false;
}