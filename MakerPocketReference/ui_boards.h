#pragma once

#include "lvgl.h"
#include "types.h"


extern BoardInfo* boards[];
extern int board_count;
extern int selected_board;
extern LegendEntry legend[8];   // if you use legend[] in detail screen


// Show the list of boards
void show_board_list();

// Show details for a specific board index
void show_board_detail(int index);