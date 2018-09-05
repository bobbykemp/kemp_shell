#include <ncurses.h>

int main(int argc, char const *argv[])
{
	initscr();
	printw("Hello world\n");
	printw("next line");
	refresh();
	getch();
	endwin();
	
	return 0;
}