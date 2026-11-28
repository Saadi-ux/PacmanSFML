// main.cpp
// The game is always in ONE state: menu, countdown, playing, win or lose.
// runGame() below looks at the state and calls the function for it (the functions are in GameState.cpp).

#include <iostream>
#include <optional>
#include <ctime>
#include <cstdlib>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

// ---------- The five states (these must be the same as in GameState.cpp) ----------
const int MENU = 0;
const int COUNTDOWN = 1;      // the 3-2-1 before play starts
const int PLAYING = 2;
const int WIN = 3;
const int LOSE = 4;

int gameState = MENU;         // the state we are in right now

// ---------- Functions needed by runGame() ----------
// GameState.cpp (one function for each state)
void runMenu(RenderWindow& window, const Font& font, Keyboard::Key key);
void runCountdown(RenderWindow& window, const Font& font);
void runPlaying(RenderWindow& window, const Font& font, Keyboard::Key key);
void runEndScreen(RenderWindow& window, const Font& font, Keyboard::Key key, bool didWin);
// sounds.cpp
void loadSounds();


// Called once per frame from main().
// This is the ONLY place that looks at gameState.
void runGame(RenderWindow& window, const Font& font, Keyboard::Key key)
{
	if (gameState == MENU)
		runMenu(window, font, key);
	else if (gameState == COUNTDOWN)
		runCountdown(window, font);
	else if (gameState == PLAYING)
		runPlaying(window, font, key);
	else if (gameState == WIN)
		runEndScreen(window, font, key, true);
	else if (gameState == LOSE)
		runEndScreen(window, font, key, false);
}


int main()
{
	srand(time(0));

	RenderWindow window(VideoMode({ 570, 680 }), "Pac-Man");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);    // one key press = one action

	Font font;
	if (!font.openFromFile("arial.ttf") && !font.openFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		cout << "Could not load a font (put arial.ttf next to maze.txt)" << endl;
		return 1;
	}

	loadSounds();

	while (window.isOpen())
	{
		// 1) EVENTS: find out which key was pressed this frame (Unknown = none)
		Keyboard::Key key = Keyboard::Key::Unknown;
		while (const optional event = window.pollEvent())
		{
			if (event->is<Event::Closed>())
				window.close();
			else if (const auto* k = event->getIf<Event::KeyPressed>())
				key = k->code;
		}

		// 2) Run the current game state, then show the frame
		window.clear(Color::Black);
		runGame(window, font, key);
		window.display();
	}

	return 0;
}