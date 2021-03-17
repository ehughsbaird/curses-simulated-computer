#include "computer.h"

#include <time.h>
#include <unistd.h>

#include "display.h"
#include "utility.h"

bool execute_instruction_register(void);

void run_program(int startadd)
{
	bool functional = true;
	computer->step = 1;
	computer->counter = startadd;
	redraw_windows();
	while (functional) {
		// Fetch
		if (computer->counter >= MEM_SIZE) {
			break;
		}
		computer->step = 0;
		computer->instruction = computer->memory[computer->counter];
		redraw_windows();
		wait_for_delay();

		// Increment
		computer->step = 1;
		++computer->counter;
		redraw_windows();
		wait_for_delay();

		// Execute
		computer->step = 2;
		functional = execute_instruction_register();
		redraw_windows();
		wait_for_delay();
	}
}

void wait_for_delay(void)
{
	// 500 ms - delay * 50ms
	// Default of half a second, min of 5% of a second
	int delay = 500000000 - computer->delay * 50000000;
	// Negative delay (computer->delay < 0) indicates step-by-step mode
	if (delay < 0) {
		while (getch() != ' ') {
		}
	} else {
		struct timespec wait;
		wait.tv_sec = 0;
		wait.tv_nsec = delay;
		nanosleep(&wait, NULL);
	}
}

static int skip_for(int address)
{
	const int reg_a = computer->reg_a;
	switch (address) {
	case 1:
		return reg_a < 0 ? 1 : 0;
	case 2:
		return reg_a > 0 ? 1 : 0;
	case 3:
		return reg_a == 0 ? 1 : 0;
	case 4:
		return reg_a <= 0 ? 1 : 0;
	case 5:
		return reg_a >= 0 ? 1 : 0;
	case 6:
		return reg_a != 0 ? 1 : 0;
	}

	return 0;
}

static int16_t raw_input(int address)
{
	while (true) {
		input_query(address);

		int16_t from_user = atoi(computer->cmd + 3);
		if (inrange(from_user, MEMVAL_MAX, MEMVAL_MIN)) {
			move_history_fwd();
			clear_cmd();
			return from_user;
		}

		clear_cmd();
	}
}

bool execute_instruction_register(void)
{
	if (!inrange(computer->instruction, MEMVAL_MAX, MEMVAL_MIN)) {
		return false;
	}
	// An instruction is 3 decimal digits, the last 2 of which are an address
	// Divide by 100 to isolate the highest order digit (the opcode)
	int opcode = computer->instruction / 100;
	// And modulus by 100 to get the address
	int address = computer->instruction % 100;

	bool modified_reg_a = false;

	if (!inrange(address, MEM_SIZE, 0)) {
		return false;
	}

	switch (opcode) {
	// Halt or skip
	case 0: {
		if (address == MEM_SIZE)
			return false;
		// halt
		if (address == 0) {
			return false;
		} else if (address < 7 && address > 0) {
			// skip, different behavior based on address
			computer->counter += skip_for(address);
		} else {
			// Invalid instruction
			return false;
		}
		break;
	}
	// Load A register with value at address
	case 1: {
		if (address == MEM_SIZE)
			return false;
		computer->reg_a = computer->memory[address];
		modified_reg_a = true;
		break;
	}
	// Store value of A register at address
	case 2: {
		if (address == MEM_SIZE)
			return false;
		computer->memory[address] = computer->reg_a;
		modified_reg_a = true;
		break;
	}
	// Add the value at address to A register
	case 3: {
		if (address == MEM_SIZE)
			return false;
		computer->reg_a += computer->memory[address];
		modified_reg_a = true;
		break;
	}
	// Subtract the value at address from A register
	case 4: {
		if (address == MEM_SIZE)
			return false;
		computer->reg_a -= computer->memory[address];
		modified_reg_a = true;
		break;
	}
	// Multiply the value of the A register by the value at address
	case 5: {
		if (address == MEM_SIZE)
			return false;
		computer->reg_a *= computer->memory[address];
		modified_reg_a = true;
		break;
	}
	// Divide the value of the A register by the value at address (integer math, round down)
	case 6: {
		if (address == MEM_SIZE)
			return false;
		computer->reg_a /= computer->memory[address];
		modified_reg_a = true;
		break;
	}
	// Input directly to memory at address
	case 7: {
		if (address == MEM_SIZE) {
			computer->reg_a = raw_input(address);
		} else {
			computer->memory[address] = raw_input(address);
		}
		break;
	}
	// Output to console
	case 8: {
		shuffle_output_fwd();
		if (address == MEM_SIZE) {
			if (!inrange(computer->reg_a, MEMVAL_MAX, MEMVAL_MIN)) {
				return false;
			}
			sprintf(computer->output[OUTPUT_SIZE - 1], "%d",
				computer->reg_a);
		} else {
			sprintf(computer->output[OUTPUT_SIZE - 1], "%d",
				computer->memory[address]);
		}
		break;
	}
	// Jump to address
	case 9: {
		if (address == MEM_SIZE)
			return false;
		computer->counter = address;
		break;
	}
	}

	if (modified_reg_a &&
	    !inrange(computer->reg_a, MEMVAL_MAX, MEMVAL_MIN)) {
		return false;
	}

	return true;
}
