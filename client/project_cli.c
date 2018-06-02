#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<inttypes.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/wait.h>

int main(int argc, char* argv[]){
    int sd[4];
    char buf[4096];
	char file[4][BUFSIZ];
    struct sockaddr_in sin;
	struct timeval t1, t2;
	int PORTNUM = atoi(argv[1]);
	int wfd, n, i, st;
	for(i=0; i<4; i++)
	{
	    if((sd[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	        perror("socket");
	        exit(1);
    	}
	}

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("210.117.184.164");

	gettimeofday(&t1, NULL);
	for(i=0; i<4; i++)
	{
		switch(fork())
		{
			case 0:
				if(connect(sd[i], (struct sockaddr *) &sin, sizeof(sin))){
					perror("connect");
   			     	exit(1);
			   	}
				sprintf(file[i], "%s.0%d", argv[2], i);

				if((wfd = open(file[i],O_CREAT|O_WRONLY|O_TRUNC, 0644))==-1){
					perror("not open file");
					exit(1);
				}

	
				if(send(sd[i], file[i], strlen(file[i])+1, 0)==-1){
					perror("send fail");
					exit(1);
				}
				while((n=read(sd[i], buf, 4096))>>0){
					if(write(wfd, buf, n)!=n){
						perror("write fail");
						exit(1);
					}
				}

				close(wfd);
				exit(0);
		}
	}
	while(wait(&st)>0);
	gettimeofday(&t2, NULL);
    printf("From Server : %s\n", argv[2]) ;
	printf("%f\n",(t2.tv_sec + t2.tv_usec*0.000001)-(t1.tv_sec + t1.tv_usec*0.000001));
    return 0;
}
