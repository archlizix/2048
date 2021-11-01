#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>

#include "game2048.h"

Game2048::Game2048()
{
	initscr(); //ncurses初始化
	cbreak(); //按键不需要输入回车直接交互
	noecho(); //按键不回显
	curs_set(0); //隐藏光标
	srand(time(NULL)); //随机数
	this->status = GameStatus::Normal;
	set_test_data();

	system("clear");
	mvprintw(LINES / 3, COLS / 3, "GAME BEGIN");
	mvprintw(LINES / 3 + 1, COLS / 3, "Press any key to entry...");
	refresh();
	getch();
}

Game2048::~Game2048()
{
	system("clear");
	mvprintw(LINES / 3, COLS / 3, "GAME END");
	mvprintw(LINES / 3 + 1, COLS / 3, "Press any key to leave...");
	refresh();

	getch();

	endwin(); //ncurses清理
}

GameStatus Game2048::get_status()
{
	return this->status;
}

void Game2048::process_input()
{
	char ch = getch();
	if ('a' <= ch && ch <= 'z') {
		ch -= 32;
	}
	if (this->status == GameStatus::Normal) {
		bool updated = false;
		if (ch == 'A') {
			updated = move_left();
		} else if (ch == 'S') {
			// 向下移动 = 旋转270度，向左移动，再旋转90度
			rotate();
			rotate();
			rotate();
			updated = move_left();
			rotate();
		} else if (ch == 'D') {
			rotate();
			rotate();
			updated = move_left();
			rotate();
			rotate();
		} else if (ch == 'W') {
			rotate();
			updated = move_left();
			rotate();
			rotate();
			rotate();
		}
		if (updated) {
			rand_new();
			if (is_over()) {
				this->status = GameStatus::Fail;
			}
		}
	}
	if (ch == 'Q') {
		this->status = GameStatus::Quit;
	} else if (ch == 'R') {
		restart();
	}
}

void Game2048::draw()
{
	// 清理屏幕
	clear();
	// 居中偏移
	const int offset = 12;
	for (int i = 0; i <= Game2048::N; ++i) {
		for (int j = 0; j <= Game2048::N; ++j) {
			// 相交点
			draw_item(i * 2, 1 + j * Game2048::WIDTH + offset, '+');
			// 竖线
			if (i != Game2048::N) {
				draw_item(i * 2 + 1, 1 + j * Game2048::WIDTH + offset, '|');
			}
			// 横线
			for (int k = 1; j != Game2048::N && k < Game2048::WIDTH; ++k) {
				draw_item(i * 2, 1 + j * Game2048::WIDTH + k + offset, '-');
			}
			// 每个格子里的数
			if (i != Game2048::N && j != Game2048::N) {
				draw_num(i * 2 + 1, (j + 1) * Game2048::WIDTH + offset, data[i][j]);
			}
		}
	}
	// 提示文字
	mvprintw(2 * Game2048::N + 2, (5 * (Game2048::N - 4) - 1) / 2,
			 "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");
	mvprintw(2 * Game2048::N + 3, 12 + 5 * (Game2048::N - 4) / 2, "archlizix");

	if (this->status == GameStatus::Win) {
		mvprintw(Game2048::N, 5 * Game2048::N / 2 - 1, " YOU WIN,PRESS R TO CONTINUE ");
	} else if (this->status == GameStatus::Fail) {
		mvprintw(Game2048::N, 5 * Game2048::N / 2 - 1, " YOU LOSE,PRESS R TO CONTINUE ");
	}
}

void Game2048::set_test_data()
{
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			data[i][j] = 16 << i << j;
		}
	}
}

// 向左边移动, 返回值表示盘面是否有发生变化
bool Game2048::move_left()
{
	int tmp[Game2048::N][Game2048::N];
	for (int i = 0; i < Game2048::N; ++i) {
		// 逐行处理
		// 如果两个数字一样，当前可写入的位置
		int current_write_pos = 0;
		int last_value = 0;
		for (int j = 0; j < Game2048::N; ++j) {
			tmp[i][j] = data[i][j];
			if (data[i][j] == 0) {
				continue;
			}
			if (last_value == 0) {
				last_value = data[i][j];
			} else {
				if (last_value == data[i][j]) {
					data[i][current_write_pos] = last_value * 2;
					last_value = 0;
					if (data[i][current_write_pos] == this->Target) {
						this->status = GameStatus::Win;
					}
				} else {
					data[i][current_write_pos] = last_value;
					last_value = data[i][j];
				}
				++current_write_pos;
			}
			data[i][j] = 0;
		}
		if (last_value != 0) {
			data[i][current_write_pos] = last_value;
		}
	}
	// 看看是否发生了变化
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			if (data[i][j] != tmp[i][j]) {
				return true;
			}
		}
	}
	return false;
}

// 矩阵逆时针旋转90度
void Game2048::rotate()
{
	int tmp[Game2048::N][Game2048::N] = {0};
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			tmp[i][j] = data[j][Game2048::N - 1 - i];
		}
	}
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			data[i][j] = tmp[i][j];
		}
	}
}

bool Game2048::is_over()
{
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			// 有空位或者相邻有一样的都可以继续
			if ((j + 1 < Game2048::N) && (data[i][j] * data[i][j + 1] == 0 || data[i][j] == data[i][j + 1])) {
				return false;
			}
			if ((i + 1 < Game2048::N) && (data[i][j] * data[i + 1][j] == 0 || data[i][j] == data[i + 1][j])) {
				return false;
			}
		}
	}
	return true;
}

void Game2048::restart()
{
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			data[i][j] = 0;
		}
	}
	rand_new();
	rand_new();
	this->status = GameStatus::Normal;
}

bool Game2048::rand_new()
{
	std::vector<int>empty_pos;
	// 把空位置先存起来
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			if (data[i][j] == 0) {
				empty_pos.push_back(i * Game2048::N + j);
			}
		}
	}
	if (empty_pos.size() == 0) {
		return false;
	}
	// 随机找个空位置
	int value = empty_pos[rand() % empty_pos.size()];
	// 10%的概率产生4
	data[value / Game2048::N][value % Game2048::N] = rand() % 10 == 1 ? 4 : 2;
	return true;
}

// 左上角为（0，0），在指定的位置画一个字符
void Game2048::draw_item(int row, int col, char c)
{
	move(row, col);
	addch(c);
}

// 游戏里的数字是右对齐，row, col是数字最后一位所在的位置
void Game2048::draw_num(int row, int col, int num)
{
	while (num > 0) {
		draw_item(row, col, num % 10 + '0');
		num /= 10;
		--col;
	}
}
