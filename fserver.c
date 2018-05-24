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

typedef struct node *Npoint;
typedef struct node{
	// Npoint parent;
	char nbuf[BUFSIZ];
	Npoint member;
	int len;
}node;

node entry[20];
Npoint np;
void preload(int len)
{
	Npoint temp;
	temp = malloc(sizeof(*temp));
	//strcpy(np->nbuf, buf);
	np->member = temp; 
	np->len = len;
	np = temp;
}
void transport(char filename[], int ns)
{
	int i; 
	Npoint np;
	for(i=0; i<20; i++)
	{
		if(!strcmp(entry[i].nbuf, filename))
		{
			for(np=entry[i].member; np->member; np=np->member)
			{	
				write(ns, np->nbuf, np->len);
			}
		}
			
	}
}


int main(int argc, char* argv[])
{
	// argv[1] = port number
	// argv[2] = open directory name
	pthread_t tid;

	int port = atoi(argv[1]);

	char buf[BUFSIZ];
	char filename[BUFSIZ];
	struct sockaddr_in sin, cli;
	int sd, clientlen = sizeof(cli);
	int i = 0, j, result, c_result;
	int ns, fd, len;


	DIR *dp;
	struct dirent *dent;
	struct stat file_chedk; // file extension check
	char path[BUFSIZ];

	if((dp = opendir(argv[2])) == NULL)
	{
		perror("Directory");
		exit(1);
	}
	
	while((dent = readdir(dp)))
	{
		j=0;
		if(dent->d_name[0] == '.') continue;
		
		np = malloc(sizeof(*np));
		strcpy((char *)entry[i].nbuf, dent->d_name); 
		printf("entry.nbuf = %s\n", dent->d_name);
		entry[i].member = np;

		sprintf(filename, "%s/%s", argv[2],dent->d_name);
		if((fd = open(filename, O_RDONLY)) == -1)
		{
			perror("file open error");
			exit(1);
		}
		
		while((len = read(fd, np->nbuf, BUFSIZ)) > 0)
		{
			// np->parent = entry[i];
			preload(len);
			j++;
			
		}
		printf("read : %d\n", j);

		
		i++;
		close(fd);
		
		
	}

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	int optvalue = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin)))
	{
		perror("bind");
		exit(1);
	}

	if(listen(sd, 5))
	{
		perror("listen");
		exit(1);
	}
	while(1)
	{
		if((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1)
		{
			perror("accept");
			exit(1);
		}
		switch(fork())
		{
			case -1:
				perror("fork error");
				exit(1);
				break;
			case 0:	
				close(sd);	// close socket server in child process
				if(recv(ns, buf, sizeof(buf), 0) == -1)
				{
					perror("recv error");
					exit(1);
				}
				transport(buf, ns);
				printf("Transport file");
				exit(0);
		}

		close(ns);
	}
	closedir(dp);
	return 0;
}
