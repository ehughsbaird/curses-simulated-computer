#pragma once
#ifndef COMP_DISPLAY_H
#define COMP_DISPLAY_H

#include <curses.h>

#include "computer.h"

void draw_memory(WINDOW *win, const computer_t *computer);
void draw_cpu(WINDOW *win, const computer_t *computer);
void draw_console(WINDOW *win, const computer_t *computer);

#endif // COMP_DISPLAY_H
