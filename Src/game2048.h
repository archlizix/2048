#pragma once

enum class GameStatus {
	Fail = 0,
	Win,
	Normal,
	Quit,
};

class Game2048
{
public:
	int Target = 2048;
	static constexpr int N = 4; //格子数
	static constexpr int WIDTH = 5; //每个格子的字符长度
	Game2048();
	~Game2048();
	GameStatus get_status();
	void process_input();
	void draw();
	void set_test_data();
private:
	int data[Game2048::N][Game2048::N];
	GameStatus status;
	bool move_left();
	void rotate();
	bool is_over();
	void restart();
	bool rand_new();
	void draw_item(int row, int col, char c);
	void draw_num(int row, int col, int num);
};
