#ifndef GUI_H
#define GUI_H

#include <string.h>

#include <ncurses.h>


void initGui() {
	initscr();
	refresh();
}

char waitInput() {
	getch();
}

void print(char* msg) {
	clear();
	printw(msg);
	refresh();
}

void killGui() {
	endwin();
}

#endif
