#pragma once

enum class GameStatus {
	fail = 0,
	win,
	normal,
	quit,
};

class Game2048
{
public:
	int Target = 2048;
	static constexpr int N = 4; //格子数
	static constexpr int WIDTH = 5; //每个格子的字符长度
	Game2048();
	~Game2048();
	GameStatus getStatus();
	void processInput();
	void draw();
	void setTestData();
private:
	int data[Game2048::N][Game2048::N];
	GameStatus status;
	bool moveLeft();
	void rotate();
	bool isOver();
	void restart();
	bool randNew();
	void drawItem(int row, int col, char c);
	void drawNum(int row, int col, int num);
};
