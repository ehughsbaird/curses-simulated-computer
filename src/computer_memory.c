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
		if (!assemble_memory(buf)) {
			if (strcmp((computer->cmd + 3), "END") == 0) {
				move_history_fwd();
				clear_cmd();
				return;
			}
			clear_cmd();
			continue;
		}
		int input = atoi(buf);
		computer->memory[add_start] = input;
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

bool assemble_memory(char *memory)
{
	int address = get_address_from(memory);

	bool is_raw = true;
	const char *cursor = memory;
	while (*cursor != '\0') {
		is_raw = is_raw && isdigit(*cursor);
		++cursor;
	}

	if (strncmp(memory, "STP", 3) == 0) {
		sprintf(memory, "000");
		return true;
	} else if (is_raw) {
		return true;
	} else if (address < 0 || address >= MEM_SIZE) {
		return false;
	} else if (strncmp(memory, "LDA", 3) == 0) {
		address += 100;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "STA", 3) == 0) {
		address += 200;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "ADD", 3) == 0) {
		address += 300;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "SUB", 3) == 0) {
		address += 400;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "MUL", 3) == 0) {
		address += 500;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "DIV", 3) == 0) {
		address += 600;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "INP", 3) == 0) {
		address += 700;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "OUT", 3) == 0) {
		address += 800;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "JMP", 3) == 0) {
		address += 900;
		sprintf(memory, "%3d", address);
		return true;
	} else if (strncmp(memory, "SKP", 3) == 0) {
		if (address > 6) {
			return false;
		}
		sprintf(memory, "%03d", address);
		return true;
	}

	return false;
}

void disassemble_memory(char *memory)
{
	if (!isdigit(*memory)) {
		return;
	}

	int address = atoi((memory + 1));
	if (address < 0 || address > MEM_SIZE) {
		return;
	}

	int opcode = *memory - '0';

	switch (opcode) {
	case 0:
		if (address != 0) {
			sprintf(memory, "STP");
		} else {
			sprintf(memory, "SKP%02d", address);
		}
		break;
	case 1:
		sprintf(memory, "LDA%02d", address);
		break;
	case 2:
		sprintf(memory, "STA%02d", address);
		break;
	case 3:
		sprintf(memory, "ADD%02d", address);
		break;
	case 4:
		sprintf(memory, "SUB%02d", address);
		break;
	case 5:
		sprintf(memory, "MUL%02d", address);
		break;
	case 6:
		sprintf(memory, "DIV%02d", address);
		break;
	case 7:
		sprintf(memory, "INP%02d", address);
		break;
	case 8:
		sprintf(memory, "OUT%02d", address);
		break;
	case 9:
		sprintf(memory, "JMP%02d", address);
		break;
	default:
		break;
	}
}
