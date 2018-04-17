#ifndef GUI_H
#define GUI_H

#include <string.h>

#include <ncurses.h>

#define BORDER 1
#define BACKGROUND 2
#define FOREGROUND 3
#define ANNOY 4
int mx, my, willWait;

char* kitteh[16];

void initGui() {
        kitteh[0] = "\\;,._                           _,,-";
        kitteh[1] = "\\`;, `-._ _..--'''```--.._ __.-',;( ";
        kitteh[2] = " \\ `;,  `:.  ,   ;.   .   :'  .;` / ";
        kitteh[3] = "  ; `;;,      .:    :.      ,;;` /  ";
        kitteh[4] = "   \\ ';/    \\:: :  . ::/    \\;` ;   ";
        kitteh[5] = "    ).' __.._`        '_..__ `./    ";
        kitteh[6] = "    /<  \\\\ /I`,      ,'I\\ //   >    ";
        kitteh[7] = "    /\\   `;-7/_\\ -- /_\\7-;'   /\\    ";
        kitteh[8] = "    //.    `\"':\" ;; \":`\"'     /\\    ";
        kitteh[9] = "     |/ .  .:' __..__ `.     \\|     ";
        kitteh[10] = "     /\\|: ./. `=_  _=' .\\   |/\\     ";
        kitteh[11] = "        /:(/::.  \\/  .::\\) /        ";
        kitteh[12] = "         ////=-v-'`-v-=\\\\\\\\         ";
        kitteh[13] = "         ///`Nx_\\;;/_xN'\\\\\\         ";
        kitteh[14] = "        / /   `\"w==w\"'   \\ \\        ";
        kitteh[15] = "         /                \\         ";

	initscr();
	noecho();
	curs_set(0);
	start_color();
	init_pair(BORDER, COLOR_BLACK, COLOR_WHITE);
	init_pair(BACKGROUND, COLOR_BLACK, COLOR_BLUE);
	init_pair(FOREGROUND, COLOR_WHITE, COLOR_BLACK);
	init_pair(ANNOY, COLOR_BLACK, COLOR_RED);
}

void setWait() {
	if (willWait != 0)
		willWait = 0;
	else
		willWait = 1;
}

void drawMessage(char* message) {
	int i;
	clear();
        getmaxyx(stdscr, my, mx);
        attron(COLOR_PAIR(BACKGROUND));
        for (int y = 0; y < my; y++)
                for (int x = 0; x < mx; x++)
                        mvaddch(y,x,' ');

        attroff(COLOR_PAIR(BACKGROUND));

        attron(COLOR_PAIR(BORDER));
        for (int y = 0; y < my; y++)
                for (int x = 0; x < mx; x++) {
                        if ( y == 1 | y == my - 2)
                                if (x > 0 && x < mx -1)
                                        mvaddch(y,x,'#');
                        if ( x == 1 | x == mx - 2)
                                if (y > 1 && y < my - 2)
                                        mvaddch(y,x,'#');
                        }

        attroff(COLOR_PAIR(BORDER));

        attron(COLOR_PAIR(FOREGROUND));
	for (i = 0; i < sizeof(kitteh)/sizeof(char*); i++) {
		move( 2 + i, (mx / 2) - strlen(kitteh[0])/2 );
		printw(kitteh[i]);
	}
        move( 2 + sizeof(kitteh)/sizeof(char*) + 2, (mx / 2) - (strlen(message) / 2));
        printw("%s",message);
        attroff(COLOR_PAIR(FOREGROUND));

	if (willWait) {
		attron(COLOR_PAIR(ANNOY));

		move(3,3);
		printw("Gosh, this sure sucks.");

		attroff(COLOR_PAIR(ANNOY));
	}
	refresh();
}

void killGui() {
	endwin();
}
#endif
