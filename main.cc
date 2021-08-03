#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>

using namespace std;

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

	char ch = 'n';
	do {
		move(5, 5);
		addch(ch);
		mvprintw(2, 2, "https://github.com/archlizix");
		ch = getch();
	} while (ch != 'Q' && ch != 'q');

	shutdown();
	return 0;
}
