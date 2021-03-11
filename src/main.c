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
	chtype input = getch();
	while (input != 'q') {
		input = getch();
	}

	delwin(memwin);
	delwin(cpuwin);
	delwin(inpwin);
	delwin(main);
	endwin();
	free(computer);
	return 1;
}
