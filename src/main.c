#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <curses.h>

#include "constants.h"
#include "computer.h"
#include "display.h"
#include "utility.h"

int main()
{
	computer_t *computer = start_computer();

	// Startup all the curses jazz
	WINDOW *main = initscr();
	keypad(main, true);
	// Though we don't actually use it
	start_color();
	// We don't need to (or want to) display the cursor
	curs_set(0);
	// We don't want to wait for a newline, take input instantly
	cbreak();
	// And we'll write the output, curses doesn't need to
	noecho();
	// We'll use this later for the computer (not quite here), so let's keep it around
	// nodelay(main, true);

	// Check we've got a big enough window
	const int term_x = window_maxx(main);
	const int term_y = window_maxy(main);
	if (term_x < DISPLAY_COLS || term_y < DISPLAY_ROWS) {
		endwin();
		free(computer);
		fprintf(stderr,
			"Window is too small (x: %d, y: %d), want (x: %d, y: %d)\n",
			term_x, term_y, DISPLAY_COLS, DISPLAY_ROWS);
		return -1;
	}

	// Start up our various displays
	WINDOW *memwin = subwin(main, MEMDISP_ROWS, MEMDISP_COLS, 0, 0);
	WINDOW *cpuwin =
		subwin(main, CPUDISP_ROWS, CPUDISP_COLS, 0, MEMDISP_COLS + 1);
	WINDOW *inpwin = subwin(main, CONSOLE_ROWS, CONSOLE_COLS, CPUDISP_ROWS,
				MEMDISP_COLS);

	wrefresh(main);
	draw_memory(memwin, computer);
	draw_cpu(cpuwin, computer);
	draw_console(inpwin, computer);

	doupdate();

	// Our main loop
	// int, because ERR is int
	// We shouldn't be recieving ERR right now, but we might in the future.
	int input = getch();
	while (input != 'q') {
		input = getch();
		mvwaddch(main, term_y - 2, term_x - 2, input);
	}

	delwin(memwin);
	delwin(cpuwin);
	delwin(inpwin);
	delwin(main);
	endwin();
	free(computer);
	return 1;
}
