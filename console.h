#pragma once

typedef struct ConsoleSize ConsoleSize;
struct ConsoleSize {
	unsigned short columns;
	unsigned short rows;
};

ConsoleSize Console_size(void);

void Console_hideCursor(void);
void Console_showCursor(void);
void Console_setCursorPosition(unsigned short row, unsigned short column);
void Console_reset(void);

void sleepMillis(unsigned long int milliseconds);
