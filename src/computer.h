#pragma once
#ifndef COMP_COMPUTER_H
#define COMP_COMPUTER_H

#include "constants.h"

typedef struct {
	int instruction;
	int counter;
	int reg_a;

	// What step of the cycle the CPU is in
	// 0 fetch instruction from memory at location counter
	// 1 increment the counter
	// 2 perform the instruction
	int step;

	int memory[MEM_SIZE];

	// Our log can hold CMDLOG_SIZE commands, and each will be CMD_SIZE characters long (NUL included)
	// cmdlog[0] is the oldest string, cmdlog[CMDLOG_SIZE - 1] the newest
	char cmdlog[CMDLOG_SIZE][CMD_SIZE];
	char cmd[CMD_SIZE];
	// Index of the first NUL in cmd
	// Just so we don't have to both searching for it every time we modify it.
	int cmd_size;
} computer_t;

// We need to access the computer and it's memory all over the place
// This makes it easier as we don't need to pass it in everywhere
extern computer_t *computer;

// Initialize computer and it's data
void start_computer(void);
// Free the memory associated with the computer
void end_computer(void);

void execute_command(computer_t *computer);

#endif // COMP_COMPUTER_H
