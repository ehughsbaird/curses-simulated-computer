#include "computer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "utility.h"

// Global computer pointer
computer_t *computer;

void start_computer(void)
{
	// Start the computer
	computer = malloc(sizeof(computer_t));

	// Do some necessary initializiations
	// Invalid data won't be displayed
	computer->instruction = MEMVAL_MAX + 1;
	computer->counter = MEMVAL_MAX + 1;
	computer->reg_a = MEMVAL_MAX + 1;

	// Always start on step 0
	computer->step = 0;

	// Fill our memory with invalid data (to prevent display of it)
	for (int i = 0; i < MEM_SIZE; ++i) {
		computer->memory[i] = MEMVAL_MAX + 1;
	}

	// Null out our command log
	for (int i = 0; i < CMDLOG_SIZE; ++i) {
		null_str(computer->cmdlog[i], CMD_SIZE);
	}

	// And our current command
	null_str(computer->cmd, CMD_SIZE);
	computer->cmd_size = 0;

	computer->delay = 9;
}

void end_computer(void)
{
	if (computer != NULL) {
		free(computer);
	}
	computer = NULL;
}

bool do_input(int backspace_limit)
{
	int input = getch();
	const int cur_cmd_size = computer->cmd_size;
	if (input == '\n' || input == KEY_ENTER) {
		return false;
	} else if (input == KEY_BACKSPACE) {
		if (cur_cmd_size > backspace_limit) {
			--computer->cmd_size;
			computer->cmd[computer->cmd_size] = '\0';
		}
	} else if (cur_cmd_size < CMD_SIZE - 1) {
		computer->cmd[cur_cmd_size] = toupper(input);
		++computer->cmd_size;
	}

	return true;
}

void enter_console(void)
{
	// The real main loop
	bool running = true;
	while (running) {
		while (do_input(0)) {
			redraw_windows();
		}
		running = execute_command();
		redraw_windows();
	}
}

void move_history_fwd(void)
{
	// Shift forward our command history
	memmove(computer->cmdlog[0], computer->cmdlog[1],
		sizeof(char) * CMD_SIZE * (CMDLOG_SIZE - 1));
	// Move our command into the history
	strcpy(computer->cmdlog[CMDLOG_SIZE - 1], computer->cmd);
}

void clear_cmd(void)
{
	// Null out our command
	null_str(computer->cmd, CMD_SIZE);
	// And set our cursor back to 0
	computer->cmd_size = 0;
}

bool execute_command(void)
{
	move_history_fwd();

	// Execute the command
	bool status = act_on_command(computer->cmd);

	clear_cmd();

	return status;
}

// Returns the 2 decimal digit address at the end of a string
int get_address_from(const char *str)
{
	int address = -1;
	const char *cursor = str;
	while (!isdigit(*cursor) && *cursor != '\0') {
		++cursor;
		if (isdigit(*cursor) && isdigit(*(cursor + 1)) &&
		    *(cursor + 2) == '\0') {
			address = atoi(cursor);
			break;
		}
	}

	return address;
}

bool act_on_command(const char *cmd)
{
	int address = get_address_from(cmd);

	if (strncmp(cmd, "LOAD", 4) == 0) {
		load_memory(address < 0 ? 0 : address);
	} else if (strcmp(cmd, "RUN") == 0) {
		run_program(0);
		clear_cmd();
	} else if (strncmp(cmd, "RUN", 3) == 0 && isdigit(cmd[3]) &&
		   isdigit(cmd[4]) && cmd[5] == '\0') {
		computer->delay = 0;
		run_program(address);
	} else if (strncmp(cmd, "RUNSPED", 7) == 0 && isdigit(cmd[7]) &&
		   cmd[8] == '\0') {
		computer->delay = cmd[7] - '0';
		run_program(0);
	} else if (strcmp(cmd, "QUIT") == 0) {
		return false;
	}

	return true;
}
