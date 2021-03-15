#pragma once
#ifndef COMP_DISPLAY_H
#define COMP_DISPLAY_H

#include <curses.h>

#include "computer.h"

bool init_windows(void);
void redraw_windows(void);
void end_windows(void);

#endif // COMP_DISPLAY_H
