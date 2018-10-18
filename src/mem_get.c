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
#define MAX_STRING 	128

#ifndef __unix
#define DEBAG

/* メモリ取得中の実行マクロ */
#define loop_func() \
	do { sleep(10); } while(0)

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
	fprintf(stderr, "USAGE : %s [-b|-k|-m] memsize\n", CMD);
	fprintf(stderr,"\n");
	exit(1);
}

/*
 * 数値妥当性関数
 */
void num_check(char *numbuf) {
	char *endptr;
	int err;

	//数値変換
	get_memsize = strtol(numbuf, &endptr, BASE_NUM);
	err = errno;
	if (*endptr != '\0' || (get_memsize == UINT_MAX && err == ERANGE)) {
		fprintf(stderr, "error invalid memory size\n");
		fprintf(stderr, "SIZE MAX : %u\n", UINT_MAX);
		goto end;
	} else if (!get_memsize) {
		fprintf(stderr, "error invalid memory size\n");
		fprintf(stderr, "SIZE MIN : 0\n");
		goto end;
	}

	return;

end:
	exit(1);
}

/*
 * シグナルハンドラ
 */
void sig_handler() {
	if (membuf) {
		free(membuf);
	}

	fprintf(stdout, "\nMEMORY GET END\n");
	exit(1);
}

/*
 * オプション解析関数
 */
void parse_opt(int argc, char **argv) {
	int c, opt_flg;
	char *numbuf;

	while ((c = getopt(argc, argv, "b;k:m:g:")) != -1) {
		switch (c) {
			case 'b':
				num_check(optarg);
				opt_flg++;
				get_memsize = get_memsize;
				break;
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

char *use_popen(void) {
	FILE *fp;
	char command[MAX_STRING];
	char output[MAX_STRING];
	sprintf(command, "grep VmSize /proc/%d/status", getpid());
	if ((fp = popen(command, "r")) == NULL) {
		perror("opne failed");
	}

	fread(output,128, 1, fp);

	if (pclose(fp) == -1) {
		perror("close failed");
	}

	return output;

}

int get_mem(unsigned int memNum) {
	char* RSS;

	//プロセスID取得
	pid_t p_pid = getppid();

	//メモリ確保
	membuf = (char*)malloc(memNum);
	if (!membuf) {
		perror("malloc failed");
		return -1;
	}
	memset(membuf, 1, memNum);

	RSS = use_popen();

	fprintf(stdout, "MEMORY GET SUCCESS!\n");
	fprintf(stdout, "PID[%d] GETSIZE[%u] RSS[%s]\n", p_pid, memNum, RSS);

	while(1) {
		loop_func();
	}

	return 0;
}

int main (int argc, char **argv) {
	int ret = 1;

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
