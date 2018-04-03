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
int draws = 0;
void drawKitteh(char* msg) {
	clear();
	char* kitteh[16];
	kitteh[0] = "\\;,._                           _,,-\n";
	kitteh[1] = "\\`;, `-._ _..--'''```--.._ __.-',;(\n";
	kitteh[2] = " \\ `;,  `:.  ,   ;.   .   :'  .;` /\n";
	kitteh[3] = "  ; `;;,      .:    :.      ,;;` /\n";
	kitteh[4] = "   \\ ';/    \\:: :  . ::/    \\;` ;\n";
	kitteh[5] = "    ).' __.._`        '_..__ `./\n";
	kitteh[6] = "    /<  \\\\ /I`,      ,'I\\ //   >\n";
	kitteh[7] = "    /\\   `;-7/_\\ -- /_\\7-;'   /\\\n";
	kitteh[8] = "    //.    `\"':\" ;; \":`\"'     /\\\n";
	kitteh[9] = "     |/ .  .:' __..__ `.     \\|\n";
	kitteh[10] = "     /\\|: ./. `=_  _=' .\   |/\\\n";
	kitteh[11] = "        /:(/::.  \\/  .::\\) /\n";
	kitteh[12] = "         ////=-v-'`-v-=\\\\\\\\\n";
	kitteh[13] = "         ///`Nx_\\;;/_xN'\\\\\\\n";
	kitteh[14] = "        / /   `\"w==w\"'   \\ \\\n";
	kitteh[15] = "         /                \\\n";
	for (int i = 0; i < 15; i++) {
		for (int i = 0; i < draws; i++)
			printw(" ");	
		printw(kitteh[i]);
	}
	printw(msg);
	refresh();
	draws++;
}

void killGui() {
	endwin();
}

#endif
