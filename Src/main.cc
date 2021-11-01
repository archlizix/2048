#include "game2048.h"

int main()
{
	Game2048 game;

	do {
		game.draw();
		game.process_input();
	} while (GameStatus::Quit != game.get_status());

	return 0;
}
