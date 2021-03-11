#include "utility.h"

int window_maxx(WINDOW *win)
{
	// I need to call the macro with a y, but I don't actually use it
	int y;
	(void)y;

	int x;
	getmaxyx(win, y, x);
	return x;
}

int window_maxy(WINDOW *win)
{
	// I need to call the macro with an x, but I don't actually use it
	int x;
	(void)x;

	int y;
	getmaxyx(win, y, x);
	return y;
}

bool inrange(int tested, int max, int min)
{
	return tested <= max && tested >= min;
}

void null_str(char *str, int size)
{
	for (int i = 0; i < size; ++i) {
		str[i] = '\0';
	}
}
