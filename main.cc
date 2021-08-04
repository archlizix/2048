#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>

using namespace std;

// 格子数
const int N = 4;
// 每个格子的字符长度
const int WIDTH = 5;
// 胜利条件
const int TARGET = 2048;

// 游戏状态
enum class game_status {
	fail = 0,
	win,
	normal,
	quit,
};

class Game2048
{
public:
	Game2048(): status(game_status::normal)
	{
		setTestData();
	}

	game_status getStatus()
	{
		return status;
	}

	// 处理按键
	void processInput()
	{
		char ch = getch();
		// 转化成大写
		if (ch >= 'a' && ch <= 'z') {
			ch -= 32;
		}
		if (status == game_status::normal) {
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
					status = game_status::fail;
				}
			}
		}
		if (ch == 'Q') {
			status = game_status::quit;
		} else if (ch == 'R') {
			restart();
		}
	}

	// 绘制游戏界面
	void draw()
	{
		// 清理屏幕
		clear();
		// 居中偏移
		const int offset = 12;
		for (int i = 0; i <= N; ++i) {
			for (int j = 0; j <= N; ++j) {
				// 相交点
				drawItem(i * 2, 1 + j * WIDTH + offset, '+');
				// 竖线
				if (i != N) {
					drawItem(i * 2 + 1, 1 + j * WIDTH + offset, '|');
				}
				// 横线
				for (int k = 1; j != N && k < WIDTH; ++k) {
					drawItem(i * 2, 1 + j * WIDTH + k + offset, '-');
				}
				// 每个格子里的数
				if (i != N && j != N) {
					drawNum(i * 2 + 1, (j + 1) * WIDTH + offset, data[i][j]);
				}
			}
		}
		// 提示文字
		mvprintw(2 * N + 2, (5 * (N - 4) - 1) / 2, "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");
		mvprintw(2 * N + 3, 12 + 5 * (N - 4) / 2, "archlizix");

		if (status == game_status::win) {
			mvprintw(N, 5 * N / 2 - 1, " YOU WIN,PRESS R TO CONTINUE ");
		} else if (status == game_status::fail) {
			mvprintw(N, 5 * N / 2 - 1, " YOU LOSE,PRESS R TO CONTINUE ");
		}
	}

	// 方便调试, 随时设置数据
	void setTestData()
	{
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				data[i][j] = 16 << i << j;
			}
		}
	}
private:
	int data[N][N];
	game_status status;

	// 向左边移动, 返回值表示盘面是否有发生变化
	bool moveLeft()
	{
		int tmp[N][N];
		for (int i = 0; i < N; ++i) {
			// 逐行处理
			// 如果两个数字一样，当前可写入的位置
			int currentWritePos = 0;
			int lastValue = 0;
			for (int j = 0; j < N; ++j) {
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
						if (data[i][currentWritePos] == TARGET) {
							status = game_status::win;
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
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				if (data[i][j] != tmp[i][j]) {
					return true;
				}
			}
		}
		return false;
	}

	// 矩阵逆时针旋转90度
	void rotate()
	{
		int tmp[N][N] = {0};
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				tmp[i][j] = data[j][N - 1 - i];
			}
		}
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				data[i][j] = tmp[i][j];
			}
		}
	}

	// 判断游戏已经结束
	bool isOver()
	{
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				// 有空位或者相邻有一样的都可以继续
				if ((j + 1 < N) && (data[i][j] * data[i][j + 1] == 0 || data[i][j] == data[i][j + 1])) {
					return false;
				}
				if ((i + 1 < N) && (data[i][j] * data[i + 1][j] == 0 || data[i][j] == data[i + 1][j])) {
					return false;
				}
			}
		}
		return true;
	}

	// 重新开始
	void restart()
	{
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				data[i][j] = 0;
			}
		}
		randNew();
		randNew();
		status = game_status::normal;
	}

	// 随机产生一个新的数字
	bool randNew()
	{
		vector<int>emptyPos;
		// 把空位置先存起来
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				if (data[i][j] == 0) {
					emptyPos.push_back(i * N + j);
				}
			}
		}
		if (emptyPos.size() == 0) {
			return false;
		}
		// 随机找个空位置
		int value = emptyPos[rand() % emptyPos.size()];
		// 10%的概率产生4
		data[value / N][value % N] = rand() % 10 == 1 ? 4 : 2;
		return true;
	}
	// 左上角为（0，0），在指定的位置画一个字符
	void drawItem(int row, int col, char c)
	{
		move(row, col);
		addch(c);
	}

	// 游戏里的数字是右对齐，row, col是数字最后一位所在的位置
	void drawNum(int row, int col, int num)
	{
		while (num > 0) {
			drawItem(row, col, num % 10 + '0');
			num /= 10;
			--col;
		}
	}
};

void initialize()
{
	// ncurses初始化
	initscr();
	// 按键不需要输入回车直接交互
	cbreak();
	// 按键不显示
	noecho();
	// 隐藏光标
	curs_set(0);
	// 随机数
	srand(time(NULL));
}

void shutdown()
{
	// ncurses清理
	endwin();
}

int main()
{
	initialize();

	Game2048 game;

	do {
		game.draw();
		game.processInput();
	} while (game_status::quit != game.getStatus());

	shutdown();
	return 0;
}
