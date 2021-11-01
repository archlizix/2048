#include "game2048.h"

int main()
{
	Game2048 game;

	do {
		game.draw();
		game.processInput();
	} while (GameStatus::quit != game.getStatus());

	return 0;
}
