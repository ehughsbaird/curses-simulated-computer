#ifndef COMP_CONSTANTS_H
#define COMP_CONSTANTS_H
#pragma once

// How the memory is display
// N columns by N rows
#define MEM_COLS 4
#define MEM_ROWS 6
// And so, this is how much memory we have
#define MEM_SIZE (MEM_COLS * MEM_ROWS)

// How many lines we save (to display) from past user commands
#define CMDLOG_SIZE 5
// How many characters are in a string (NUL included!)
#define CMD_SIZE 9

// What is the maximum value that can be stored
#define MEMVAL_MAX 999
// And the minimum
#define MEMVAL_MIN -999

// The largest decimal number representable in MEMADD_SIZE digits
// Just to shut up sprintf about invalid formatting
#define ADD_MAX 99

// The size of (decimal digits) an address in memory for display
// MEM_SIZE should probably be representable in this many decimal digits
#define MEMADD_SIZE 2
// The size of (decimal digits) a value in memory for display
// MEMVAL_MAX and MEMVAL_MIN should probably be representable in this many decimal digits
#define MEMCLL_SIZE 5

// +2 for the separators
// The size of a single box displaying the address and value of a location in memory
#define MEMBOX_COLS (MEMADD_SIZE + MEMCLL_SIZE + 2)
// The offset of the separator after an address in a MEMBOX
#define MEMBOX_ADD_OFFSET (1 + MEMADD_SIZE)

// The number of columns in the entire memory display window
// Add two to each for dividers, 1 for the final divider
#define MEMDISP_COLS (MEM_COLS * MEMBOX_COLS + 1)
// The number of rows in the entire memory display window
// Each data row has a divider, but 1 to cap the end
#define MEMDISP_ROWS (MEM_ROWS * 2 + 1)

// The number of columns in the entire cpu display window
// 3 separators + 2 character register names
#define CPUDISP_COLS (5 + MEMCLL_SIZE)
// The number of rows in the entire CPU display window
// 3 possible stages, 3 registers, 3 separators
#define CPUDISP_ROWS 9

// The number of columns in the entire console display window
// 2 Separators, 1 prompt, CMD_SIZE - 1 characters per command
#define CONSOLE_COLS (2 + CMD_SIZE)
// The number of rows in the entire console display window
// 2 separators, 1 prompt line, 5 in the log
#define CONSOLE_ROWS (3 + CMDLOG_SIZE)

// As many lines as the console, but no extra command
#define OUTPUT_SIZE (1 + CMDLOG_SIZE)

// The number of columns in composite of all the display windows
#define DISPLAY_COLS (CONSOLE_COLS + MEMDISP_COLS)
// The number of rows in the composite of all the display windows
// We're making the assumption here other options aren't tweaked to make
// MEMDISP_ROWS Shorter than CONSOLE_ROWS + CPUDISP_ROWS
#define DISPLAY_ROWS (2 * CONSOLE_COLS)

#endif // COMP_CONSTANTS_H
