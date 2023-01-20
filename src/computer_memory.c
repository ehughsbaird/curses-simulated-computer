#include "computer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"

void load_memory(int16_t add_start)
{
	while (add_start < MEM_SIZE) {
		input_query(add_start);

		char buf[CMD_SIZE];
		strcpy(buf, computer->cmd + 3);

		bool all_digit = true;
		char *c;

		struct memcell instr = assemble_memory(buf);

		for(c = buf; c < buf + CMD_SIZE && *c != '\0'; ++c) {
			all_digit = all_digit && isdigit(*c);
		}
		if(all_digit) {
			load_memory_val(add_start, atoi(buf));
		} else if (instr.valid) {
			load_memory_ins(add_start, instr.value);
		} else if(strcmp((computer->cmd + 3), "END") == 0) {
			move_history_fwd();
			clear_cmd();
			return;
		} else {
			clear_cmd();
			continue;
		}

		++add_start;
		move_history_fwd();
		clear_cmd();
	}
}

void input_query(int address)
{
	// Input > 2 chars should be illegal
	sprintf(computer->cmd, "%02d:", address);
	computer->cmd_size = 3;

	redraw_windows();
	while (do_input(3)) {
		redraw_windows();
	}
}

struct memcell assemble_memory(char *memory)
{
	struct memcell ret;
	ret.valid = true;
	ret.instruction = true;
	int address = get_address_from(memory);

	if (strncmp(memory, "STP", 3) == 0) {
		ret.value = 0;
	} else if (address < 0 || address > MEM_SIZE) {
		ret.valid = false;
	} else if (strncmp(memory, "INP", 3) == 0) {
		ret.value = 700 + address;
	} else if (strncmp(memory, "OUT", 3) == 0) {
		ret.value = 800 + address;
	} else if (address == MEM_SIZE) {
		ret.valid = false;
	} else if (strncmp(memory, "LDA", 3) == 0) {
		ret.value = 100 + address;
	} else if (strncmp(memory, "STA", 3) == 0) {
		ret.value = 200 + address;
	} else if (strncmp(memory, "ADD", 3) == 0) {
		ret.value = 300 + address;
	} else if (strncmp(memory, "SUB", 3) == 0) {
		ret.value = 400 + address;
	} else if (strncmp(memory, "MUL", 3) == 0) {
		ret.value = 500 + address;
	} else if (strncmp(memory, "DIV", 3) == 0) {
		ret.value = 600 + address;
	} else if (strncmp(memory, "JMP", 3) == 0) {
		ret.value = 900 + address;
	} else if (strncmp(memory, "SKP", 3) == 0) {
		if (address > 6) {
			ret.valid = false;
		} else {
			ret.value = address;
		}
	}

	return ret;
}

void disassemble_memory(char *out, int size, int memory)
{
	int address = memory % 100;
	if (address < 0 || address > MEM_SIZE) {
		return;
	}

	int opcode = memory / 100;

	switch (opcode) {
	case 0:
		if (address != 0) {
			snprintf(out, size, "STP");
		} else {
			snprintf(out, size, "SKP%02d", address);
		}
		break;
	case 1:
		snprintf(out, size, "LDA%02d", address);
		break;
	case 2:
		snprintf(out, size, "STA%02d", address);
		break;
	case 3:
		snprintf(out, size, "ADD%02d", address);
		break;
	case 4:
		snprintf(out, size, "SUB%02d", address);
		break;
	case 5:
		snprintf(out, size, "MUL%02d", address);
		break;
	case 6:
		snprintf(out, size, "DIV%02d", address);
		break;
	case 7:
		snprintf(out, size, "INP%02d", address);
		break;
	case 8:
		snprintf(out, size, "OUT%02d", address);
		break;
	case 9:
		snprintf(out, size, "JMP%02d", address);
		break;
	default:
		break;
	}
}
