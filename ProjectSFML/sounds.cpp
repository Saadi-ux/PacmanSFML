//#include <iostream>
//#include <optional>
//#include <SFML/Audio.hpp>
//using namespace std;
//using namespace sf;
//
//// The order here must match the SND_ constants in main.cpp
//const int NUM_SOUNDS = 9;
//const char* soundFiles[NUM_SOUNDS] = {
//	"chomp.wav", "power.wav", "eatghost.wav", "death.wav", "beep.wav",
//	"go.wav", "menu.wav", "win.wav", "gameover.wav"
//};
//
//SoundBuffer buffers[NUM_SOUNDS];
//optional<Sound> sounds[NUM_SOUNDS];
//
//// Loads every .wav once at startup. A missing file just means that sound stays silent.
//void loadSounds()
//{
//	for (int i = 0; i < NUM_SOUNDS; i++)
//	{
//		if (buffers[i].loadFromFile(soundFiles[i]))
//			sounds[i].emplace(buffers[i]);
//		else
//			cout << "Could not load " << soundFiles[i] << endl;
//	}
//}
//
//void playSound(int id)
//{
//	if (id < 0 || id >= NUM_SOUNDS || !sounds[id])
//		return;
//	sounds[id]->stop();
//	sounds[id]->play();
//}
