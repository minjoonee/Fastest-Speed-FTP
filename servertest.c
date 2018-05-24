#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>


int main(int argc, char *argv[])
{
	pthread_t tid;

	int portnum = atoi(argv[1]);

	char buf[BUFSIZ];
	char filename[BUFSIZ];
	struct sockaddr_in sin, cli;
	int sockfd, clientlen = sizeof(cli);
	int i, j;
	int ns, fd, len;

	DIR *dp;
	struct dirent *dent;
	struct stat file_check;
	int kind;
	char path[BUFSIZ];

	if((dp = opendir(argv[2])) == NULL)
	{
		perror("Directory");
		exit(1);
	}

	while(dent = readdir(dp))
	{
		if(dent->d_name[0] == '.') continue;
		
		switch(fork())
		{
			case -1:
				perror("first fork error");
				exit(1);
				break;
			case 0:
				// 1/4 split -> split -n 4 path filename 
				sprintf(path, "%s/%s", argv[2], dent->d_name);
				if(execlp("split", "split", "-n", "4", path, dent->d_name, NULL) == -1)
				{
					perror("execlp");
					exit(1);
				}
				exit(0);
		}
	}

	return 0;
}
