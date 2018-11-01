CC=gcc
CFLAGS=-I. -lm
OBJ = *.c

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

a.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)