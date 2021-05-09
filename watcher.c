#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>
#define BUF_SIZE 1024
#define CHANNEL "THE_CHANNEL_ID"
#define TOKEN "THE_SLACK_TOKEN"
#define PREFIX "token="TOKEN"&channel="CHANNEL"&text="

void post_to_slack(CURL * curl, char *buffer)
{
	CURLcode res;
	char *msg;
	char *payload;

	if ((msg = curl_easy_escape(curl, buffer, 0)) == NULL)
		return;

	if ((payload = malloc(sizeof(PREFIX) + strlen(msg))) == NULL) {
		perror("malloc");
		goto error;
	}
	strcpy(payload, PREFIX);
	strcat(payload, msg);
	curl_easy_setopt(curl, CURLOPT_URL,
			 "https://slack.com/api/chat.postMessage");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}
	fflush(stdout);
	free(payload);
 error:
	curl_free(msg);
}

int main(int ac, char **av)
{
	int inotify_fd;
	int fd;
	struct inotify_event event;
	CURL *curl;

	if (ac < 2) {
		fprintf(stderr, "Usage: watcher FILE\n");
		return EXIT_FAILURE;
	}

	/* Init curl */
	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		fprintf(stderr, "Cannot init curl\n");
		return EXIT_FAILURE;
	}
	if ((curl = curl_easy_init()) == NULL) {
		fprintf(stderr, "Cannot init curl\n");
		return EXIT_FAILURE;
	}

	/* Event notification */
	inotify_fd = inotify_init();
	if (inotify_fd == -1) {
		perror("inotify_init");
		return EXIT_FAILURE;
	}
	if ((fd = open(av[1], O_RDONLY)) == -1) {
		fprintf(stderr, "Cannot open %s\n", av[1]);
		perror("open");
		return EXIT_FAILURE;
	}
	if (inotify_add_watch(inotify_fd, av[1], IN_MODIFY) == -1) {
		fprintf(stderr, "Cannot watch %s\n", av[1]);
		perror("inotify_add_watch");
		return EXIT_FAILURE;
	}
	if (lseek(fd, 0, SEEK_END) == -1) {
		perror("lseek");
		return EXIT_FAILURE;
	}
	for (;;) {
		ssize_t ret;

		if ((ret = read(inotify_fd, &event, sizeof(event))) > 0) {
			char buffer[BUF_SIZE];
			ssize_t n;

			while ((n = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
				buffer[n] = '\0';
				post_to_slack(curl, buffer);
			}
			if (n == -1) {
				perror("read");
				return EXIT_FAILURE;
			}
		} else if (ret == -1 && errno != EINTR) {
			perror("read");
			return EXIT_FAILURE;
		}
	}
	curl_easy_cleanup(curl);
	close(fd);
	close(inotify_fd);
	return EXIT_SUCCESS;
}
