#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>

#define CMD 		"mem_get"
#define BASE_NUM	10
#define DEBAG

/* ユーザ定義関数の宣言 */
void usage();
void num_check(char *numbuf);
void sig_handler(); 

/* グローバル変数 */
static char *membuf;
static unsigned int get_memsize;
static const char *const memcg1_stat_names[] = {
	"cache",
	"rss",
	"rss_huge",
	"shmem",
	"mapped_file",
	"dirty",
	"writeback",
	"swap",
};

void usage() {
	fprintf(stderr, "USAGE : %s [-k|-m] memsize\n", CMD);
	fprintf(stderr,"\n");
	exit(1);
}

void num_check(char *numbuf) {
	char *endptr;
	int err;

	//数値変換
	get_memsize = strtol(numbuf, &endptr, BASE_NUM);
	err = errno;
	if (*endptr != '\0' || (get_memsize == UINT_MAX && err == ERANGE)) {
		fprintf(stderr, "error invalid memory size\n");
		fprintf(stderr, "SIZE MAX : %u\n", UINT_MAX);
		exit(1);
	}
}

void sig_handler() {
	if (membuf) {
		free(membuf);
	}

	fprintf(stdout, "\nMEMORY GET END\n");
	exit(1);
}

void parse_opt(int argc, char **argv) {
	int c, opt_flg;
	char *numbuf;

	while ((c = getopt(argc, argv, "k:m:g:")) != -1) {
		switch (c) {
			case 'k':
				num_check(optarg);
				opt_flg++;
				get_memsize = get_memsize * 1024;
				break;
			case 'm':
				num_check(optarg);
				opt_flg++;
				get_memsize = get_memsize * 1048576;
				break;
			default:
				usage();
		}		break;
	}

	//オプション重複排除/指定数値の妥当性確認
	if(1 < opt_flg || get_memsize == 0 || argc < 2) {
		usage();
	}

}

int get_mem(unsigned int memNum) {
	//プロセスID取得
	pid_t p_pid = getppid();

	//メモリ確保
	membuf = (char*)malloc(memNum);
	if (!membuf) {
		perror("malloc failed");
		return -1;
	}
	memset(membuf, 1, memNum);
	
	
	fprintf(stdout, "MEMORY GET SUCCESS!\n");
	fprintf(stdout, "PID[%d] GETSIZE[%u]\n", p_pid, memNum);

	while(1) {
		sleep(10);
	}

	return 0;
}

int main (int argc, char **argv) {
	int ret;

	//オプション解析
	parse_opt(argc, argv);

	if(signal(SIGINT, sig_handler) == SIG_ERR) {
		fprintf(stderr, "signal set failed\n");
		return -1;
	}

	//メモリ取得
	ret = get_mem(get_memsize);
	if(!ret) 
		usage();

	return 0;
}
