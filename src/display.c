#include "display.h"

#include <assert.h>

#include "utility.h"

bool curses_init();

void draw_memory(WINDOW *win, const computer_t *computer);
void draw_cpu(WINDOW *win, const computer_t *computer);
void draw_console(WINDOW *win, const computer_t *computer);
void draw_output(WINDOW *win, const computer_t *computer);
void draw_info(WINDOW *win);

// So we don't have to pass these all over
WINDOW *memory_window = NULL;
WINDOW *cpu_window = NULL;
WINDOW *console_window = NULL;
WINDOW *output_window = NULL;
WINDOW *main_window = NULL;
WINDOW *info_window = NULL;

bool init_windows()
{
	// Startup all the curses jazz
	main_window = initscr();

	if (main_window == NULL) {
		return false;
	}

	bool gui_success = curses_init();

	// Start up our various displays
	if (gui_success) {
		memory_window =
			subwin(main_window, MEMDISP_ROWS, MEMDISP_COLS, 0, 0);
		cpu_window = subwin(main_window, CPUDISP_ROWS, CPUDISP_COLS,
				    CONSOLE_ROWS, MEMDISP_COLS + 1);
		console_window =
			subwin(main_window, CONSOLE_ROWS, CONSOLE_COLS,
			       CPUDISP_ROWS + CONSOLE_ROWS, MEMDISP_COLS);
		output_window = subwin(main_window, CONSOLE_ROWS, CONSOLE_COLS,
				       0, MEMDISP_COLS);

		if (window_maxx(main_window) >= DISPLAY_COLS + INFO_COLS) {
			info_window = subwin(main_window, INFO_ROWS, INFO_COLS,
					     0, DISPLAY_COLS);
		}

		redraw_windows();
	}

	return gui_success;
}

void redraw_windows()
{
	draw_memory(memory_window, computer);
	draw_cpu(cpu_window, computer);
	draw_console(console_window, computer);
	draw_output(output_window, computer);
	if (info_window) {
		draw_info(info_window);
	}

	doupdate();
}

void end_windows()
{
	if (memory_window != NULL) {
		delwin(memory_window);
	}

	if (cpu_window != NULL) {
		delwin(cpu_window);
	}

	if (console_window != NULL) {
		delwin(console_window);
	}

	if (output_window != NULL) {
		delwin(output_window);
	}

	if (main_window != NULL) {
		delwin(main_window);
	}

	if (info_window != NULL) {
		delwin(info_window);
	}

	memory_window = NULL;
	cpu_window = NULL;
	console_window = NULL;
	output_window = NULL;
	info_window = NULL;
	main_window = NULL;
	endwin();
}

bool curses_init()
{
	// We need to ensure there is input here
	keypad(main_window, true);
	// Though we don't actually use it
	start_color();
	// We don't need to (or want to) display the cursor
	curs_set(0);
	// We don't want to wait for a newline, take input instantly
	cbreak();
	// And we'll write the output, curses doesn't need to
	noecho();
	// We'll use this later for the computer (not quite here), so let's keep it around
	// nodelay(main_window, true);

	// Check we've got a big enough window
	const int term_x = window_maxx(main_window);
	const int term_y = window_maxy(main_window);
	if (term_x < DISPLAY_COLS || term_y < DISPLAY_ROWS) {
		endwin();
		fprintf(stderr,
			"Window is too small (x: %d, y: %d), want (x: %d, y: %d)\n",
			term_x, term_y, DISPLAY_COLS, DISPLAY_ROWS);
		return false;
	}

	return true;
}

void draw_info(WINDOW *win)
{
	// INFO_LEN x INFO_DEPTH
	werase(win);
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	mvwaddnstr(win, 1, 1, "> Curses Simulated Computer", INFO_LEN);
	mvwaddnstr(win, 2, 1, "", INFO_LEN);
	mvwaddnstr(win, 3, 1, "Assembly manual:", INFO_LEN);
	mvwaddnstr(win, 4, 1, "LDAXX: AR = value at XX", INFO_LEN);
	mvwaddnstr(win, 5, 1, "STAXX: value at XX = AR", INFO_LEN);
	mvwaddnstr(win, 6, 1, "ADDXX: AR = AR + value at XX", INFO_LEN);
	mvwaddnstr(win, 7, 1, "SUBXX: AR = AR - value at XX", INFO_LEN);
	mvwaddnstr(win, 8, 1, "MULXX: AR = AR * value at XX", INFO_LEN);
	mvwaddnstr(win, 9, 1, "DIVXX: AR = AR / value at XX", INFO_LEN);
	mvwaddnstr(win, 10, 1, "INPXX: Input value at XX", INFO_LEN);
	mvwaddnstr(win, 11, 1, "OUTXX: Print value at XX", INFO_LEN);
	mvwaddnstr(win, 12, 1, "JMPXX: Jump to XX", INFO_LEN);
	mvwaddnstr(win, 13, 1, "For INP & OUT, XX=24 for AR", INFO_LEN);
	mvwaddnstr(win, 14, 1, "SKPX: 00 Halt, 0[1-6] jump 1", INFO_LEN);
	mvwaddnstr(win, 15, 1, "0X: 1 <  0, 2 >  0, 3  = 0", INFO_LEN);
	mvwaddnstr(win, 16, 1, "0X: 4 <= 0, 5 >= 0, 6 != 0", INFO_LEN);
	mvwaddnstr(win, 17, 1, "", INFO_LEN);
	mvwaddnstr(win, 18, 1, "Commands: LOAD, RUN, QUIT", INFO_LEN);
	wrefresh(win);
}

void draw_memory(WINDOW *win, const computer_t *computer)
{
	// Clean up whatever is left from drawing this previously
	werase(win);
	// Draw a border around it
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);

	/*
	 * We draw lines at
	 *     3     9 12    18 21    27 30
	 *  |--|-----|--|-----|--|-----|--|-----|
	 *  |ad|memry|ad|memry|ad|memry|ad|memry|
	 * 2-------------------------------------
	 *  |ad|memry|ad|memry|ad|memry|ad|memry|
	 * 4-------------------------------------
	 *  |ad|memry|ad|memry|ad|memry|ad|memry|
	 * 6-------------------------------------
	 *  |ad|memry|ad|memry|ad|memry|ad|memry|
	 * 8-------------------------------------
	 *  |ad|memry|ad|memry|ad|memry|ad|memry|
	 *
	 * So where x % 9 == 0 and x % 9 == 3
	 * And y % 2 == 0
	 *
	 * Or, more generalized
	 * where x % MEMBOX_COLS == 0 and x % MEMBOX_COLS == MEMBOX_ADD_OFFSET
	 * and where y % 2 == 0
	 */
	const int maxx = window_maxx(win);
	const int maxy = window_maxy(win);

	// Layout the table
	for (int y = 0; y < maxy; ++y) {
		for (int x = 0; x < maxx; ++x) {
			const bool yzero = y == 0;
			const bool ymax = y == maxy - 1;
			const bool xzero = x == 0;
			const bool xmax = x == maxx - 1;
			const bool draw_vline =
				(x % MEMBOX_COLS == 0 ||
				 x % MEMBOX_COLS == MEMBOX_ADD_OFFSET);
			const bool draw_hline = y % 2 == 0;
			const bool on_boundary = yzero || ymax || xzero || xmax;

			if (!xzero && !xmax && yzero && draw_vline) {
				// The first line, connect the lines we're drawing down to the top
				mvwaddch(win, y, x, ACS_TTEE);
			} else if (!xzero && !xmax && ymax && draw_vline) {
				// Now, connect the lines we're drawing down to the bottom
				mvwaddch(win, y, x, ACS_BTEE);
			} else if (xzero && !(yzero || ymax) && draw_hline) {
				// And connect the sides (from the left)
				mvwaddch(win, y, x, ACS_LTEE);
			} else if (xmax && !(yzero || ymax) && draw_hline) {
				// And connect the side (from the right)
				mvwaddch(win, y, x, ACS_RTEE);
			} else if (!on_boundary && draw_hline && draw_vline) {
				// Draw the in-between connectors
				mvwaddch(win, y, x, ACS_PLUS);
			} else if (!on_boundary && draw_vline) {
				// Now, draw the vertical lines
				mvwaddch(win, y, x, ACS_VLINE);
			} else if (!on_boundary && draw_hline) {
				// And the horizontal ones
				mvwaddch(win, y, x, ACS_HLINE);
			}
		}
	}

	// Draw the addresses and memory
	// We'll be using these for everything we print, allocate them once
	char add_buf[40];
	char mem_buff[40];
	for (int y = 1; y < maxy - 1; y += 2) {
		for (int x = 0; x < MEM_COLS; ++x) {
			// We draw memory top to bottom, left to right
			// So 6x gives us the amount of cells in the columns before us
			// And y / 2 will round down to the number of cells above us
			int idx = MEM_ROWS * x + (y / 2);
			// Each address starts MEMBOX_COLS apart from eachother, offset 1 from the edge
			const int add_coord = MEMBOX_COLS * x + 1;
			// Each cell starts MEMBOX_COLS apart from eachother, offset MEDADD_SIZE + 1 from the edge
			// The comment above is wrong, for some reason
			const int mem_coord = MEMBOX_COLS * x + MEMADD_SIZE + 2;
			const struct memcell memory = computer->memory[idx];

			// Make sure sprintf doesn't complain about idx being too large
			// (If its bigger, it won't fit in add_buf)
			if (idx > ADD_MAX) {
				idx = ADD_MAX;
			}

			// This is a valid value for memory, and it will format nicely
			const bool memory_valid = memory.valid;
			const bool decode_memory = memory.instruction;

			sprintf(add_buf, "%02d", idx);
			if (memory_valid && !decode_memory) {
				char sign = ' ';
				if (memory.value < 0) {
					sign = '-';
				}

				// We're drawing our own sign character!
				sprintf(mem_buff, " %c%03d", sign, ABS(memory.value));
			}
			if (decode_memory) {
				disassemble_memory(mem_buff, MEMCLL_SIZE + 1, memory.value);
			}

			// But because sprintf will still complain sometimes, take only the first SIZE + 1 characters from the string.
			// (The characters to display the address, and NUL)
			add_buf[MEMADD_SIZE] = '\0';

			// Use addnstr to prevent values from leaking past their positions
			// I know the format specifiers in sprintf should do the same
			// I'm just being careful
			mvwaddnstr(win, y, add_coord, add_buf, MEMADD_SIZE);
			// Values > MEMVAL_MAX or < MEMVAL_MIN are invalid
			if (memory_valid) {
				mvwaddnstr(win, y, mem_coord, mem_buff,
					   MEMCLL_SIZE);
			}
		}
	}

	// Finally, draw the window
	wrefresh(win);
}

void draw_cpu(WINDOW *win, const computer_t *computer)
{
	// Clear the window to prevent leftovers
	werase(win);
	// Give it a nice border
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);

	/*
	 * Here's our layout
	 * 0123456789
	 * ----------0
	 * |EXEC  | |1
	 * |INCR  |X|2
	 * |FTCH  | |3
	 * ----------4
	 * |IR|-0000|5
	 * |PC|-0000|6
	 * |AR|-0000|7
	 * ----------8
	 *
	 *  The first box indicates if we are in execute/increment/fetch with an X
	 *  fetch is abbreviated to match the others
	 *
	 *  The second shows the values in:
	 *  IR - instruction register
	 *  PC - program counter
	 *  AR - A register
	 *
	 *  Keep in mind! This will be different if MEMCLL_SIZE changes!
	 */

	// Draw the horizontal lines
	// First! We'll overwrite them with the special line characters below
	for (int x = 1; x < window_maxx(win) - 1; ++x) {
		mvwaddch(win, 4, x, ACS_HLINE);
	}

	// Draw some special line characters at (x,y) (7,0),(3,4),(7,4),(3,9)
	mvwaddch(win, 0, 7, ACS_TTEE);
	mvwaddch(win, 4, 7, ACS_BTEE);
	mvwaddch(win, 4, 3, ACS_TTEE);
	mvwaddch(win, 8, 3, ACS_BTEE);

	// Draw the vertical lines
	// This could be done in exactly as many lines with a for loop
	mvwaddch(win, 1, 7, ACS_VLINE);
	mvwaddch(win, 2, 7, ACS_VLINE);
	mvwaddch(win, 3, 7, ACS_VLINE);
	mvwaddch(win, 5, 3, ACS_VLINE);
	mvwaddch(win, 6, 3, ACS_VLINE);
	mvwaddch(win, 7, 3, ACS_VLINE);

	// Draw the cycle strings
	mvwaddstr(win, 1, 1, "FTCH");
	mvwaddstr(win, 2, 1, "INCR");
	mvwaddstr(win, 3, 1, "EXEC");
	// And the indicator
	mvwaddch(win, computer->step + 1, 8, 'X');

	// Draw the register strings
	mvwaddstr(win, 5, 1, "IR");
	mvwaddstr(win, 6, 1, "PC");
	mvwaddstr(win, 7, 1, "AR");

	// Use a simple buffer for all the registers
	char regbuf[40];

	// Then print their values 1 by 1
	// Using addnstr just to be extra careful, sprintf should handle format problems
	static_assert(MEMCLL_SIZE == 5,
		      "Remember to update the format strings here!");

	if (inrange(computer->instruction, MEMVAL_MAX, MEMVAL_MIN)) {
		disassemble_memory(regbuf, 6, computer->instruction);
		mvwaddnstr(win, 5, 4, regbuf, 5);
	}

	if (inrange(computer->counter, MEMVAL_MAX, MEMVAL_MIN)) {
		char sign = ' ';
		if (computer->counter < 0) {
			sign = '-';
		}
		sprintf(regbuf, " %c%03d", sign, computer->counter);
		mvwaddnstr(win, 6, 4, regbuf, 5);
	}

	if (inrange(computer->reg_a, MEMVAL_MAX, MEMVAL_MIN)) {
		char sign = ' ';
		if (computer->reg_a < 0) {
			sign = '-';
		}
		// We're adding our own sign character!
		int16_t reg_a = ABS(computer->reg_a);
		sprintf(regbuf, " %c%03d", sign, reg_a);
		mvwaddnstr(win, 7, 4, regbuf, 5);
	}

	// Finally, draw the window
	wrefresh(win);
}

void draw_console(WINDOW *win, const computer_t *computer)
{
	// Make sure there's nothing left over
	werase(win);
	// And give use a nice border around the console
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);

	/*
	 * 01234567890
	 * -----------0
	 * |         |1
	 * |         |2
	 * |         |3
	 * |         |4
	 * |         |5
	 * |?        |6
	 * -----------7
	 * 5 lines for previous entries, one for the current entry
	 */

	// Fill out the log
	for (int i = 0; i < CMDLOG_SIZE; ++i) {
		// Offset of 1,1 to not overwrite the borders
		mvwaddstr(win, i + 1, 1, computer->cmdlog[i]);
	}

	// Now draw our current command, and the prompt
	mvwaddch(win, CMDLOG_SIZE + 1, 1, '?');
	mvwaddstr(win, CMDLOG_SIZE + 1, 2, computer->cmd);

	// Finally, draw the window
	wrefresh(win);
}

void draw_output(WINDOW *win, const computer_t *computer)
{
	// Make sure there's nothing left over
	werase(win);
	// And give use a nice border around the console
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);

	/*
	 * 01234567890
	 * -----------0
	 * |         |1
	 * |         |2
	 * |         |3
	 * |         |4
	 * |         |5
	 * |         |6
	 * -----------7
	 * 5 lines for previous entries, one for the current entry
	 */

	// Fill out the log
	for (int i = 0; i < OUTPUT_SIZE; ++i) {
		// Offset of 1,1 to not overwrite the borders
		mvwaddstr(win, i + 1, 1, computer->output[i]);
	}

	// Finally, draw the window
	wrefresh(win);
}
