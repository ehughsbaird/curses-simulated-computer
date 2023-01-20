#pragma once
#ifndef COMP_COMPUTER_H
#define COMP_COMPUTER_H

#define _XOPEN_SOURCE 600

#include <stdbool.h>
#include <stdint.h>

#include "constants.h"

struct memcell {
	// Interpret these first two as usigned so they hold 0, 1 not 0, -1
	// Valid bit encodes whether this piece of memory has been set
	uint16_t valid : 1;
	// Controls display of this piece of memory, whether or not to decode it as an instruction
	uint16_t instruction : 1;
	// Empty space, reserve for future use
	int16_t padding : 3;
	// Memory can hold values from -999 to 999
	int16_t value : 11;
	_Static_assert(MEMVAL_MIN == -999 && MEMVAL_MAX == 999, "Adjust memcell sizing if MEMVAL size changes!");
};
_Static_assert(sizeof(struct memcell) == sizeof(int16_t), "Size check");

typedef struct {
	int16_t instruction;
	int16_t counter;
	int16_t reg_a;

	// What step of the cycle the CPU is in
	// 0 fetch instruction from memory at location counter
	// 1 increment the counter
	// 2 perform the instruction
	int step;

	struct memcell memory[MEM_SIZE];

	// Our log can hold CMDLOG_SIZE commands, and each will be CMD_SIZE characters long (NUL included)
	// cmdlog[0] is the oldest string, cmdlog[CMDLOG_SIZE - 1] the newest
	char cmdlog[CMDLOG_SIZE][CMD_SIZE];
	char cmd[CMD_SIZE];
	// Index of the first NUL in cmd
	// Just so we don't have to both searching for it every time we modify it.
	int cmd_size;

	char output[OUTPUT_SIZE][CMD_SIZE];

	int delay;
} computer_t;

// We need to access the computer and it's memory all over the place
// This makes it easier as we don't need to pass it in everywhere
extern computer_t *computer;

// Initialize computer and it's data
void start_computer(void);
// Free the memory associated with the computer
void end_computer(void);

// Do necessary cleanup in the log and command, the run a command
bool execute_command(void);

// Transfer control to the console
void enter_console(void);

// Actually run command cmd
bool act_on_command(const char *cmd);

// Load a value between MEMVAL_MIN and MEMVAL_MAX into memory
void load_memory_val(int addr, int val);

// Load an assembled instruction into memory
void load_memory_ins(int addr, int ins);

// Read a memory cell, including meta-data
struct memcell read_memory(int addr);

// Read the value in a memory cell
int16_t read_memory_val(int addr);

// false = input is done, execute
// true  = continue doing input
bool do_input(int backspace_limit);

// If memory is a valid assembly instruction:
// 	create a corresponding memcell with valid data set
//
// Otherwise, return a memcell without valid data set
struct memcell assemble_memory(char *memory);

// Turn machine code back in assembly instructions, in place
// (If it's a valid instruction)
void disassemble_memory(char *out, int size, int memory);

// Get a 2 char address from str
int get_address_from(const char *str);

// Run the program in memory, starting add start_add
void run_program(int start_add);

// Load input into memory
void load_memory(int16_t add_start);

// Query for input into computer memory
void input_query(int address);

// Move the command history forwards, copy the entered command into it
// (DOES NOT CLEAR THE CURRENT COMMAND)
void move_history_fwd(void);
// Clear the current command
void clear_cmd(void);

void shuffle_output_fwd(void);

void wait_for_delay(void);

#endif // COMP_COMPUTER_H
