.POSIX:

watcher: watcher.c
	$(CC) -g -o $@ -Wall -Wextra $< -lcurl
