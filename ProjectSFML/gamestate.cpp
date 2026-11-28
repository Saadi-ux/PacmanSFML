// GameState.cpp
// The code for each state (each "case" of the game): menu, countdown, playing, win and lose.
// runGame() in main.cpp looks at gameState and calls the right function from this file.

#include <SFML/Graphics.hpp>

using namespace sf;

const int ROWS = 40;
const int COLS = 40;
const float TILE = 30.f;

// ---------- The five states (these must be the same as in main.cpp) ----------
const int MENU = 0;
const int COUNTDOWN = 1;      // the 3-2-1 before play starts
const int PLAYING = 2;
const int WIN = 3;
const int LOSE = 4;

extern int gameState;         // the state we are in right now (defined in main.cpp)

// ---------- Menu options ----------
const int MENU_NEW = 0;
const int MENU_CONTINUE = 1;
const int MENU_EXIT = 2;

// ---------- Sound ids (same order as soundFiles in sounds.cpp) ----------
const int SND_CHOMP = 0, SND_POWER = 1, SND_GHOST = 2, SND_DEATH = 3, SND_BEEP = 4;
const int SND_GO = 5, SND_MENU = 6, SND_WIN = 7, SND_GAMEOVER = 8;

// ---------- Game variables ----------
char maze[ROWS][COLS];
int prow = 0, pcol = 0;       // player row and column
int dir = 0;                  // player direction
int startRow = 0, startCol = 0;
int score = 0;
int lives = 3;

int menuChoice = MENU_NEW;    // which menu option is highlighted
bool noSaveMsg = false;       // true = show "No saved game" on the menu
int countdown = 3;            // the number shown during the countdown (3, 2, 1)
int countFrames = 0;          // frames since that number appeared (60 frames = 1 second)

extern int Px;
extern int Py;

// ---------- Functions that live in the other .cpp files ----------
// Maze.cpp
bool loadMaze(const char* filename, char maze[ROWS][COLS]);
void initGhosts(char maze[ROWS][COLS]);
void drawMaze(RenderWindow& window, int& prow, int& pcol, int dir, char maze[ROWS][COLS]);
// Player.cpp
void readInput(int& dir, int prow, int pcol, char maze[ROWS][COLS]);
int movePlayer(int dir, int& prow, int& pcol, char maze[ROWS][COLS]);
// Ghost.cpp
void initGhostState();
bool updateGhosts(char maze[ROWS][COLS], int prow, int pcol, int oldPRow, int oldPCol, int& score, int& lives);
// Game.cpp (saving and loading)
void findPlayer(int& prow, int& pcol, char maze[ROWS][COLS]);
void saveGame(char maze[ROWS][COLS], int score, int lives, int prow, int pcol);
bool loadGame(char maze[ROWS][COLS], int& score, int& lives, int& prow, int& pcol);
bool hasSave();
void deleteSave();
// drawing.cpp
void syncDrawState(char maze[ROWS][COLS]);
void drawHUD(RenderWindow& window, const Font& font, int score, int lives);
void drawMenu(RenderWindow& window, const Font& font, int choice, bool saveExists, bool showNoSave);
void drawCountdown(RenderWindow& window, const Font& font, int number);
void drawEndScreen(RenderWindow& window, const Font& font, bool won, int score);
// sounds.cpp
void playSound(int id);


// =====================================================
//  SMALL HELPER FUNCTIONS
// =====================================================

// How many tiles in the maze are this character?
// countTiles(maze, '.') = how many dots are left
int countTiles(char maze[ROWS][COLS], char ch)
{
	int total = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (maze[r][c] == ch)
				total++;
	return total;
}

// Put the player back on the start tile
void placePlayerAtStart()
{
	prow = startRow;
	pcol = startCol;
	Px = (int)(pcol * TILE + TILE / 2);
	Py = (int)(prow * TILE + TILE / 2);
	dir = 0;
}

// Draw the maze and the score/lives bar (every state except the menu does this)
void drawGame(RenderWindow& window, const Font& font)
{
	drawMaze(window, prow, pcol, dir, maze);
	drawHUD(window, font, score, lives);
}

// Switch to the COUNTDOWN state
void startCountdown()
{
	countdown = 3;
	countFrames = 0;
	gameState = COUNTDOWN;
}

// Switch to the WIN state
void winGame()
{
	gameState = WIN;
	deleteSave();
	playSound(SND_WIN);
}

// Switch to the LOSE state
void loseGame()
{
	gameState = LOSE;
	deleteSave();
	playSound(SND_GAMEOVER);
}

// New Game: score 0, 3 lives, and a full maze loaded again from maze.txt
bool startNewGame()
{
	if (!loadMaze("maze.txt", maze))
		return false;
	initGhosts(maze);
	initGhostState();

	findPlayer(startRow, startCol, maze);
	placePlayerAtStart();

	score = 0;
	lives = 3;
	syncDrawState(maze);
	return true;
}

// Continue: start a new game first, then put the saved game on top of it
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


// =====================================================
//  ONE FUNCTION FOR EACH STATE
//  Each one does 3 things: check the key, update the game, draw the screen
// =====================================================

void runMenu(RenderWindow& window, const Font& font, Keyboard::Key key)
{
	// 1) check the key
	if (key == Keyboard::Key::Up || key == Keyboard::Key::W)
	{
		menuChoice--;
		if (menuChoice < MENU_NEW)
			menuChoice = MENU_EXIT;       // went past the top, jump to the bottom
		noSaveMsg = false;
		playSound(SND_MENU);
	}
	else if (key == Keyboard::Key::Down || key == Keyboard::Key::S)
	{
		menuChoice++;
		if (menuChoice > MENU_EXIT)
			menuChoice = MENU_NEW;        // went past the bottom, jump to the top
		noSaveMsg = false;
		playSound(SND_MENU);
	}
	else if (key == Keyboard::Key::Enter)
	{
		if (menuChoice == MENU_NEW)
		{
			if (startNewGame())
				startCountdown();         // -> COUNTDOWN state
			else
				window.close();           // maze.txt is missing
		}
		else if (menuChoice == MENU_CONTINUE)
		{
			if (hasSave() && continueGame())
				startCountdown();         // -> COUNTDOWN state
			else
				noSaveMsg = true;         // no save, so stay on the menu
		}
		else
			window.close();               // Exit
	}

	// 2) draw the screen
	drawMenu(window, font, menuChoice, hasSave(), noSaveMsg);
}

void runCountdown(RenderWindow& window, const Font& font)
{
	// 1) draw the screen
	drawGame(window, font);
	drawCountdown(window, font, countdown);

	// 2) update the timer
	if (countFrames == 0)
		playSound(SND_BEEP);              // beep when each number appears

	countFrames++;
	if (countFrames == 60)                // 60 frames = 1 second
	{
		countFrames = 0;
		countdown--;                      // 3 -> 2 -> 1 -> 0
	}

	if (countdown == 0)
	{
		playSound(SND_GO);
		gameState = PLAYING;              // -> PLAYING state
	}
}

void runPlaying(RenderWindow& window, const Font& font, Keyboard::Key key)
{
	// 1) check the key: Escape saves and goes back to the menu
	if (key == Keyboard::Key::Escape)
	{
		saveGame(maze, score, lives, prow, pcol);
		gameState = MENU;                 // -> MENU state
		return;
	}

	// 2) update the game
	int oldRow = prow, oldCol = pcol;     // (the ghosts need to know where the player was)

	readInput(dir, prow, pcol, maze);
	int points = movePlayer(dir, prow, pcol, maze);   // points for the pellet eaten (0 if none)
	score += points;

	if (points == 50)
		playSound(SND_POWER);
	else if (points == 10)
		playSound(SND_CHOMP);

	int pelletsLeft = countTiles(maze, '.') + countTiles(maze, 'o');
	int scoreBeforeGhosts = score;

	if (pelletsLeft == 0)
		winGame();                        // -> WIN state
	else if (updateGhosts(maze, prow, pcol, oldRow, oldCol, score, lives))
	{
		// a ghost caught the player
		placePlayerAtStart();
		if (lives <= 0)
			loseGame();                   // -> LOSE state
		else
			playSound(SND_DEATH);
	}
	else if (score > scoreBeforeGhosts)
		playSound(SND_GHOST);             // the player ate a blue ghost

	// 3) draw the screen
	drawGame(window, font);
}

// Used by both the WIN state and the LOSE state
void runEndScreen(RenderWindow& window, const Font& font, Keyboard::Key key, bool didWin)
{
	// 1) check the key: Enter goes back to the menu
	if (key == Keyboard::Key::Enter)
	{
		gameState = MENU;                 // -> MENU state
		return;
	}

	// 2) draw the screen
	drawGame(window, font);
	drawEndScreen(window, font, didWin, score);
}