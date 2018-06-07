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
	strcpy(file, argv[2]);	// filename is argv[2]
	if(!strcmp("2.fna",argv[2])||!strcmp("kernel.tar",argv[2])){	// if file is fna or tar,
		sprintf(file,"%s.zip",argv[2]);	// file name is argv[2].zip
		zip_count++;	// zip file count.
	}
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){	// Create a socket descriptor.
        perror("socket");
        exit(1);
    }
	// Initialize socket
    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("210.117.184.164");

    if(connect(sd, (struct sockaddr *) &sin, sizeof(sin))){	// connect server.
        perror("connect");
        exit(1);
    }
	if(send(sd, file, strlen(file)+1, 0)==-1){	// send the file name to download
		perror("send fail");
		exit(1);
	}

	if((wfd = open(file,O_CREAT|O_WRONLY|O_TRUNC, 0644))==-1){	// Creat and open file.
        perror("fopen");
		exit(1);
	}
	gettimeofday(&t1, NULL);
	while((n=read(sd, buf, BUFSIZ))>0){	// Receive file data
		if(write(wfd, buf, n)!=n){	// write file
				perror("write fail");
				exit(1);
		}
	}
	gettimeofday(&t2, NULL);

	close(wfd);	// close file
    close(sd);	// close socket
	printf("%f\n",(t2.tv_sec + t2.tv_usec*0.000001)-(t1.tv_sec + t1.tv_usec*0.000001));
	if(zip_count>0){	// received a zip file
		switch(fork()){	// use fork()
		case 0:
			if(execlp("unzip","unzip",file,(char *)NULL)==-1)	// Use execlp to uncompress 
				perror("execlp");
		}
	}
    return 0;
}
