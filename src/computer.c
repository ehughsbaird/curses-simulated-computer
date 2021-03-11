#include "computer.h"

#include <stdlib.h>
#include <string.h>

#include "utility.h"

computer_t *start_computer(void)
{
	// Start the computer
	computer_t *computer = malloc(sizeof(computer_t));

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

	// And out current command
	null_str(computer->cmd, CMD_SIZE);
	computer->cmd_size = 0;

	return computer;
}

void execute_command(computer_t *computer)
{
	// Shift forward our command history
	memmove(computer->cmdlog[0], computer->cmdlog[1],
		sizeof(char) * CMD_SIZE * (CMDLOG_SIZE - 1));
	// Move our command into the history
	strcpy(computer->cmdlog[CMDLOG_SIZE - 1], computer->cmd);
	// TODO: Do something based on the command
	// Null out our command
	null_str(computer->cmd, CMD_SIZE);
	// And set our cursor back to 0
	computer->cmd_size = 0;
}
