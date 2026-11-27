#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using namespace std;
using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;
const float HUD_H = 50.f;

// The game is always in exactly one of these states
const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;      // shared by "GAME OVER" and "YOU WIN" (see the 'won' flag)

int gameState = MENU;
int menuChoice = 0;           // 0 = New Game, 1 = Continue, 2 = Exit
bool won = false;
bool noSaveMsg = false;      // shows "No saved game" on the menu

char maze[ROWS][COLS];
int prow = 0, pcol = 0, dir = 0;
int startRow = 0, startCol = 0;
int score = 0, lives = 3;

extern int Px;
extern int Py;

bool loadMaze(const char* filename, char maze[ROWS][COLS]);
void drawMaze(RenderWindow& window, int& prow, int& pcol, int dir, char maze[ROWS][COLS]);
void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS]);
void movePlayer(int dir, int& prow, int& pcol, char maze[ROWS][COLS]);
void initGhosts(char maze[ROWS][COLS]);
void initGhostState();
bool updateGhosts(char maze[ROWS][COLS], int prow, int pcol, int oldPRow, int oldPCol, int& score, int& lives);
void frightenGhosts();
void findPlayer(int& prow, int& pcol, char maze[ROWS][COLS]);
void saveGame(char maze[ROWS][COLS], int score, int lives, int prow, int pcol);
bool loadGame(char maze[ROWS][COLS], int& score, int& lives, int& prow, int& pcol);
bool hasSave();
void deleteSave();
void syncDrawState(char maze[ROWS][COLS]);
void drawHUD(RenderWindow& window, const Font& font, int score, int lives);
void drawEndScreen(RenderWindow& window, const Font& font, bool won, int score);
void drawMenu(RenderWindow& window, const Font& font, int choice, bool saveExists, bool showNoSave);

// Counts the normal pellets ('.') and the power pellets ('o') left in the maze
void countPellets(char maze[ROWS][COLS], int& dots, int& power)
{
	dots = 0;
	power = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
		{
			if (maze[r][c] == '.') dots++;
			else if (maze[r][c] == 'o') power++;
		}
}

// New Game: score 0, 3 lives, and a full maze loaded again from maze.txt
bool startNewGame()
{
	if (!loadMaze("maze.txt", maze))
		return false;
	initGhosts(maze);
	initGhostState();

	findPlayer(prow, pcol, maze);
	startRow = prow;
	startCol = pcol;
	Px = (int)(pcol * TILE + TILE / 2);
	Py = (int)(prow * TILE + TILE / 2);

	dir = 0;
	score = 0;
	lives = 3;
	won = false;
	syncDrawState(maze);
	return true;
}

// Continue: set up a fresh game first (so the start cells are known), then put the saved game on top
bool continueGame()
{
	if (!startNewGame())
		return false;
	if (!loadGame(maze, score, lives, prow, pcol))
	{
		deleteSave();             // the save could not be read, so throw it away
		return false;
	}
	dir = 0;
	syncDrawState(maze);
	return true;
}

// ---------- input: one key press, handled by the current state ----------
void handleKey(RenderWindow& window, Keyboard::Key key)
{
	switch (gameState)
	{
	case MENU:
		if (key == Keyboard::Key::Up || key == Keyboard::Key::W)
		{
			menuChoice = (menuChoice + 2) % 3;
			noSaveMsg = false;
		}
		else if (key == Keyboard::Key::Down || key == Keyboard::Key::S)
		{
			menuChoice = (menuChoice + 1) % 3;
			noSaveMsg = false;
		}
		else if (key == Keyboard::Key::Enter)
		{
			if (menuChoice == 0)
			{
				if (startNewGame())
					gameState = PLAYING;
				else
					window.close();           // maze.txt is missing
			}
			else if (menuChoice == 1)
			{
				if (hasSave() && continueGame())
					gameState = PLAYING;
				else
					noSaveMsg = true;         // stay on the menu
			}
			else
				window.close();
		}
		break;

	case PLAYING:
		if (key == Keyboard::Key::Escape)
		{
			saveGame(maze, score, lives, prow, pcol);
			gameState = MENU;
		}
		break;

	case GAME_OVER:
		if (key == Keyboard::Key::Enter)
		{
			deleteSave();
			gameState = MENU;
		}
		break;
	}
}

// ---------- update: one frame of game logic, for the current state ----------
void update()
{
	switch (gameState)
	{
	case MENU:
		break;

	case PLAYING:
	{
		int oldRow = prow, oldCol = pcol;

		int dotsBefore, powerBefore;
		countPellets(maze, dotsBefore, powerBefore);

		readInput(dir, prow, pcol, maze);
		movePlayer(dir, prow, pcol, maze);

		int dotsAfter, powerAfter;
		countPellets(maze, dotsAfter, powerAfter);
		score += (dotsBefore - dotsAfter) * 10 + (powerBefore - powerAfter) * 50;

		if (dotsAfter + powerAfter == 0)
		{
			won = true;                   // all pellets eaten
			gameState = GAME_OVER;
			deleteSave();
		}
		else if (updateGhosts(maze, prow, pcol, oldRow, oldCol, score, lives))
		{
			prow = startRow;
			pcol = startCol;
			Px = (int)(pcol * TILE + TILE / 2);
			Py = (int)(prow * TILE + TILE / 2);
			dir = 0;

			if (lives <= 0)
			{
				won = false;              // out of lives
				gameState = GAME_OVER;
				deleteSave();
			}
		}
		break;
	}

	case GAME_OVER:
		break;
	}
}

// ---------- draw: one frame, for the current state ----------
void draw(RenderWindow& window, const Font& font)
{
	window.clear(Color::Black);

	switch (gameState)
	{
	case MENU:
		drawMenu(window, font, menuChoice, hasSave(), noSaveMsg);
		break;

	case PLAYING:
		drawMaze(window, prow, pcol, dir, maze);
		drawHUD(window, font, score, lives);
		break;

	case GAME_OVER:
		drawMaze(window, prow, pcol, dir, maze);
		drawHUD(window, font, score, lives);
		drawEndScreen(window, font, won, score);
		break;
	}
}

int main()
{
	srand(time(0));

	RenderWindow window(VideoMode({ 570, 680 }), "Pac-Man");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);    // one Enter press = one action

	Font font;
	if (!font.openFromFile("arial.ttf") && !font.openFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		cout << "Could not load a font (put arial.ttf next to maze.txt)" << endl;
		return 1;
	}

	while (window.isOpen())
	{
		while (const optional event = window.pollEvent())
		{
			if (event->is<Event::Closed>())
				window.close();
			else if (const auto* key = event->getIf<Event::KeyPressed>())
				handleKey(window, key->code);
		}

		update();
		draw(window, font);
		window.display();
	}

	return 0;
}