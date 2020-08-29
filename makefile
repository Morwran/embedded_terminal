
CC = gcc
CFLAGS = -c -std=c99 -Wall
LDFLAGS=
SUBDIR = terminal/
SOURCES= buffers.c terminal.c terminal_commands.c uart.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=

	
all: $(OBJECTS)
	#$(CC) buffers.o terminal.o terminal_commands.o uart.o -o terminals.o

buffers.o: $(SUBDIR)buffers.c 
	$(CC) $(CFLAGS)	$(SUBDIR)buffers.c

terminal.o: $(SUBDIR)terminal.c
	$(CC) $(CFLAGS)	$(SUBDIR)terminal.c

terminal_commands.o: $(SUBDIR)terminal_commands.c
	$(CC) $(CFLAGS)	$(SUBDIR)terminal_commands.c

uart.o: $(SUBDIR)uart.c
	$(CC) $(CFLAGS)	$(SUBDIR)uart.c	

clean:
	rm -rf *.o	