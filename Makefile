.POSIX:

watcher: watcher.c
	$(CC) -o $@ -Wall -Wextra $<
