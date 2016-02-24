CC ?= gcc
CFLAGS_common ?= -O0 -Wall -std=gnu99

make:
	$(CC) sender.c -o sender
	$(CC) receiver.c -o receiver

clean:
	$(RM) sender
	$(RM) receiver
	$(RM) receive.jpg
	$(RM) receive.mp3
	$(RM) receive.mp4

