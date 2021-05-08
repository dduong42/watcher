#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>

int main(int ac, char **av)
{
	int inotify_fd = inotify_init();
	int *fds = calloc(ac - 1, sizeof(*fds));
	int i;

	if (fds == NULL) {
		perror("calloc");
		return EXIT_FAILURE;
	}

	if (inotify_fd == -1) {
		perror("inotify_init");
		return EXIT_FAILURE;
	}
	for (i = 0; i < ac - 1; i++) {
		char *path = av[i+1];
		if ((fds[i] = open(path, O_RDONLY)) == -1) {
			fprintf(stderr, "Cannot open %s\n", av[i]);
			perror("open");
			return EXIT_FAILURE;
		}
		if (inotify_add_watch(inotify_fd, path, IN_MODIFY) == -1) {
			fprintf(stderr, "Cannot watch %s\n", av[i]);
			perror("inotify_add_watch");
		}
	}
	for (i = 0; i < ac - 1; i++) {
		close(fds[i]);
	}
	close(inotify_fd);
	free(fds);
	return EXIT_SUCCESS;
}
