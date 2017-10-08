CC=gcc
program: x_input_grab.o
	$(CC) -o x_input_grab.exe x_input_grab.c `pkg-config --cflags --libs x11-xcb x11`
