#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "constants.h"
#include "computer.h"
#include "display.h"
#include "utility.h"

int main(void)
{
	start_computer();

	if (!init_windows()) {
		end_windows();
		end_computer();
		return -1;
	}

	// Our main loop
	// int, because ERR is int
	// We shouldn't be recieving ERR right now, but we might in the future.
	int input = getch();
	while (input != 'q') {
		const int cur_cmd_size = computer->cmd_size;
		if (input == '\n' || input == KEY_ENTER) {
			execute_command(computer);
		} else if (input == KEY_BACKSPACE) {
			if (cur_cmd_size > 0) {
				--computer->cmd_size;
				computer->cmd[computer->cmd_size] = '\0';
			}
		} else if (cur_cmd_size < CMD_SIZE - 1) {
			computer->cmd[cur_cmd_size] = input;
			++computer->cmd_size;
		}

		redraw_windows();
		input = getch();
	}

	end_windows();
	end_computer();
	return 1;
}
