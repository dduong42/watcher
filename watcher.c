#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>

void post_to_slack(char *buffer)
{
	/*
	CURL *curl = curl_easy_init();

	if (curl) {
		CURLcode res;

	}*/
	printf("%s", buffer);
}

int main(int ac, char **av)
{
	int inotify_fd;
	FILE *file;
	struct inotify_event event;

	if (ac < 2) {
		fprintf(stderr, "Usage: watcher FILE");
		return EXIT_FAILURE;
	}
	inotify_fd = inotify_init();
	if (inotify_fd == -1) {
		perror("inotify_init");
		return EXIT_FAILURE;
	}
	if ((file = fopen(av[1], "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", av[1]);
		perror("open");
		return EXIT_FAILURE;
	}
	if (inotify_add_watch(inotify_fd, av[1], IN_MODIFY) == -1) {
		fprintf(stderr, "Cannot watch %s\n", av[1]);
		perror("inotify_add_watch");
		return EXIT_FAILURE;
	}
	if (fseek(file, 0, SEEK_END) == -1) {
		perror("fseek");
		return EXIT_FAILURE;
	}
	for (;;) {
		if (read(inotify_fd, &event, sizeof(event)) == -1 && errno != EINTR) {
			perror("read");
			return EXIT_FAILURE;
		} else {
			char buffer[1024];

			while (fgets(buffer, sizeof(buffer), file) != NULL) {
				post_to_slack(buffer);
			}
		}
	}
	fclose(file);
	close(inotify_fd);
	return EXIT_SUCCESS;
}
