#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define CMD "cpu_get"

int loopy () {
	int i;
	fprintf(stdout, "thread !!\n");
	return 0;
}

void usage() {
	fprintf(stderr, "usage : %s thread_num\n", CMD);
	exit(1);
}

void errmsg(char *msg, int err) {
	fprintf(stderr, "%s : (%d)\n", msg, err);
	exit(1);
}

int main (int argc, char **argv) {
	int thread_num;
	int ret, err;
	pthread_t *thread;

	thread_num = atoi(argv[1]);

	thread = malloc(sizeof(thread) * thread_num);
	if (!thread) {
		err = errno;
		errmsg("malloc faild", err);
	}

	for (int i = 0; i < thread_num; i++) {
		ret = pthread_create(&thread[i], NULL, (void *)loopy, NULL);
		if (ret) {
			fprintf(stderr, "thread create failed(%d)\n", errno);
			exit(1);
		}
		ret = pthread_join(thread[i], NULL);
		if (ret) {
			err = errno;
			errmsg("can not join thread", err);
		}
	}
	if (thread)
		free(thread);
}
