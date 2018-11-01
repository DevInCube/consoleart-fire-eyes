#define _DEFAULT_SOURCE
#define _POSIX_SOURCE	

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>

#include "console.h"

ConsoleSize Console_size(void) {
	struct winsize w = {0};
    ioctl(0, TIOCGWINSZ, &w);
	return (ConsoleSize) {
		.columns = w.ws_col,
		.rows = w.ws_row
	};
}

void sleepMillis(unsigned long int milliseconds) {
#ifdef __linux__
	struct timespec tim;
	tim.tv_sec = (milliseconds / 1000L);
	tim.tv_nsec = (milliseconds % 1000L) * 1000000L;

	nanosleep(&tim, NULL);
#else
	Sleep(milliseconds);
#endif
}

void Console_hideCursor(void) {
	printf("\033[?25l");
	fflush(stdout);
}

void Console_showCursor(void) {
	printf("\033[?25h");
	fflush(stdout);
}

void Console_setCursorPosition(unsigned short row, unsigned short column) {
	printf("\033[%i;%iH", row, column);
	fflush(stdout);
}

void Console_reset(void) {
	printf("\033[%im", 0);
	fflush(stdout);
}