#include "computer.h"

#include <stdlib.h>

#include "utility.h"

computer_t *start_computer()
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
