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
int main(int argc, char* argv[]){
    int sd;
    char buf[BUFSIZ];
	char file[BUFSIZ];
	char ip_buf[BUFSIZ];
	struct sockaddr_in sin;
	struct timeval t1, t2;
	int PORTNUM = atoi(argv[1]);
	int wfd, n;
	int zip_count=0;
	strcpy(file, argv[2]);
	if(!strcmp("2.fna",argv[2])||!strcmp("kernel.tar",argv[2])){
		sprintf(file,"%s.zip",argv[2]);
		zip_count++;
	}
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("210.117.184.164");

    if(connect(sd, (struct sockaddr *) &sin, sizeof(sin))){
        perror("connect");
        exit(1);
    }
	if(send(sd, file, strlen(file)+1, 0)==-1){
		perror("send fail");
		exit(1);
	}

	if((wfd = open(file,O_CREAT|O_WRONLY|O_TRUNC, 0644))==-1){
        perror("fopen");
		exit(1);
	}
	printf("open %s\n",file);

	gettimeofday(&t1, NULL);
	while((n=read(sd, buf, BUFSIZ))>>0){
		if(write(wfd, buf, n)!=n){
				perror("write fail");
				exit(1);
		}
	}
	gettimeofday(&t2, NULL);

    if(recv(sd, ip_buf, sizeof(ip_buf), 0) == -1){
        perror("recv");
        exit(1);
    }
	close(wfd);
    close(sd);
	printf("%f\n",(t2.tv_sec + t2.tv_usec*0.000001)-(t1.tv_sec + t1.tv_usec*0.000001));
	if(zip_count>0){
		switch(fork()){
		case 0:
			if(execlp("unzip","unzip",file,(char *)NULL)==-1)
				perror("execlp");
		}
	}
    return 0;
}
