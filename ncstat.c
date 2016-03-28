#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "cJSON.h"

#define SIZE (1024*64)

static char addr[16] = "127.0.0.1";
static int port = 22222;

int get_data_from_twemproxy(int argc, char *argv[], void *buf)
{ 
	int sockfd, size, ret = 0;
	struct hostent *he;
	struct sockaddr_in server;

	if ((he = gethostbyname(addr)) == NULL) {
		printf("gethostbyname error\n");
		return -1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket() error \n");
		return -1;
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr = *((struct in_addr *)he->h_addr);

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
		printf("connetc() error\n");
		return -1;
	}

	size = recv(sockfd, (void *)buf, SIZE, 0);
	if (size < 0) {
		perror("recv");
	}

	if (ret = close(sockfd)) {
		perror("close");
	}

	return size;
}

void show_json(char *buf, int size)
{
	cJSON *cj = cJSON_Parse(buf);
	char *stat = cJSON_Print(cj);

	if (cj == NULL)
		return;

	if (stat != NULL) {
		printf("nutcracker stat :\n");
		printf("%s\n", stat);
		free(stat);
	}

	cJSON_Delete(cj);
}

void show_usage()
{
	printf("Usage :\n");
	printf("-a,   : set stats monitoring ip (default: 127.0.0.1)\n");
	printf("-s,   : set stats monitoring port (default: 22222)\n\n");
	exit(0);
}

void parse_command(int argc, char *argv[])
{
	int opt = 0;

	while ((opt = getopt(argc, argv, "a:s:h")) != -1) {
		switch (opt) {
			case 'a' :
				strncpy(addr, optarg, sizeof(addr));
				break;

			case 't' :
				port = atoi(optarg);
				break;

			case 'h' :
			default : 
				show_usage();
		}
	}
}

int main ( int argc, char *argv[])
{
	void *buf = NULL;

	parse_command(argc, argv);

	buf = calloc(1, SIZE);
	int size = get_data_from_twemproxy(argc, argv, buf);
	if (size > 0) {
		show_json((char*)buf, size);
	}

	return 0;
}
