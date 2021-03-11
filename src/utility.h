#pragma once
#ifndef COMP_UTILITY_H
#define COMP_UTILITY_H

#include "stdlib.h"

#include <curses.h>

int window_maxx(WINDOW *win);
int window_maxy(WINDOW *win);

bool inrange(int tested, int max, int min);

void null_str(char *str, int size);

#endif // COMP_UTILITY_H
