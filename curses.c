#include <ncurses.h>

int main(int argc, char const *argv[])
{
	initscr();
	printw("Hello world");
	refresh();
	getch();
	endwin();
	
	return 0;
}