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

	// Now, pass over control to our computer
	enter_console();

	end_windows();
	end_computer();
	return 1;
}
