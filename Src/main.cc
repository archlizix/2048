#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>

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

Game2048::Game2048()
{
	initscr(); //ncurses初始化
	cbreak(); //按键不需要输入回车直接交互
	noecho(); //按键不回显
	curs_set(0); //隐藏光标
	srand(time(NULL)); //随机数
	this->status = GameStatus::normal;
	setTestData();

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

GameStatus Game2048::getStatus()
{
	return status;
}

void Game2048::processInput()
{
	char ch = getch();
	if (ch >= 'a' && ch <= 'z') {
		ch -= 32;
	}
	if (status == GameStatus::normal) {
		bool updated = false;
		if (ch == 'A') {
			updated = moveLeft();
		} else if (ch == 'S') {
			// 向下移动 = 旋转270度，向左移动，再旋转90度
			rotate();
			rotate();
			rotate();
			updated = moveLeft();
			rotate();
		} else if (ch == 'D') {
			rotate();
			rotate();
			updated = moveLeft();
			rotate();
			rotate();
		} else if (ch == 'W') {
			rotate();
			updated = moveLeft();
			rotate();
			rotate();
			rotate();
		}
		if (updated) {
			randNew();
			if (isOver()) {
				status = GameStatus::fail;
			}
		}
	}
	if (ch == 'Q') {
		status = GameStatus::quit;
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
			drawItem(i * 2, 1 + j * Game2048::WIDTH + offset, '+');
			// 竖线
			if (i != Game2048::N) {
				drawItem(i * 2 + 1, 1 + j * Game2048::WIDTH + offset, '|');
			}
			// 横线
			for (int k = 1; j != Game2048::N && k < Game2048::WIDTH; ++k) {
				drawItem(i * 2, 1 + j * Game2048::WIDTH + k + offset, '-');
			}
			// 每个格子里的数
			if (i != Game2048::N && j != Game2048::N) {
				drawNum(i * 2 + 1, (j + 1) * Game2048::WIDTH + offset, data[i][j]);
			}
		}
	}
	// 提示文字
	mvprintw(2 * Game2048::N + 2, (5 * (Game2048::N - 4) - 1) / 2,
			 "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");
	mvprintw(2 * Game2048::N + 3, 12 + 5 * (Game2048::N - 4) / 2, "archlizix");

	if (status == GameStatus::win) {
		mvprintw(Game2048::N, 5 * Game2048::N / 2 - 1, " YOU WIN,PRESS R TO CONTINUE ");
	} else if (status == GameStatus::fail) {
		mvprintw(Game2048::N, 5 * Game2048::N / 2 - 1, " YOU LOSE,PRESS R TO CONTINUE ");
	}
}

void Game2048::setTestData()
{
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			data[i][j] = 16 << i << j;
		}
	}
}

// 向左边移动, 返回值表示盘面是否有发生变化
bool Game2048::moveLeft()
{
	int tmp[Game2048::N][Game2048::N];
	for (int i = 0; i < Game2048::N; ++i) {
		// 逐行处理
		// 如果两个数字一样，当前可写入的位置
		int currentWritePos = 0;
		int lastValue = 0;
		for (int j = 0; j < Game2048::N; ++j) {
			tmp[i][j] = data[i][j];
			if (data[i][j] == 0) {
				continue;
			}
			if (lastValue == 0) {
				lastValue = data[i][j];
			} else {
				if (lastValue == data[i][j]) {
					data[i][currentWritePos] = lastValue * 2;
					lastValue = 0;
					if (data[i][currentWritePos] == this->Target) {
						status = GameStatus::win;
					}
				} else {
					data[i][currentWritePos] = lastValue;
					lastValue = data[i][j];
				}
				++currentWritePos;
			}
			data[i][j] = 0;
		}
		if (lastValue != 0) {
			data[i][currentWritePos] = lastValue;
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

bool Game2048::isOver()
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
	randNew();
	randNew();
	status = GameStatus::normal;
}

bool Game2048::randNew()
{
	std::vector<int>emptyPos;
	// 把空位置先存起来
	for (int i = 0; i < Game2048::N; ++i) {
		for (int j = 0; j < Game2048::N; ++j) {
			if (data[i][j] == 0) {
				emptyPos.push_back(i * Game2048::N + j);
			}
		}
	}
	if (emptyPos.size() == 0) {
		return false;
	}
	// 随机找个空位置
	int value = emptyPos[rand() % emptyPos.size()];
	// 10%的概率产生4
	data[value / Game2048::N][value % Game2048::N] = rand() % 10 == 1 ? 4 : 2;
	return true;
}

// 左上角为（0，0），在指定的位置画一个字符
void Game2048::drawItem(int row, int col, char c)
{
	move(row, col);
	addch(c);
}

// 游戏里的数字是右对齐，row, col是数字最后一位所在的位置
void Game2048::drawNum(int row, int col, int num)
{
	while (num > 0) {
		drawItem(row, col, num % 10 + '0');
		num /= 10;
		--col;
	}
}

int main()
{
	Game2048 game;

	do {
		game.draw();
		game.processInput();
	} while (GameStatus::quit != game.getStatus());

	return 0;
}
