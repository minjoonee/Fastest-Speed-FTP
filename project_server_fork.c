#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<inttypes.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<fcntl.h>
#define BUF 4096

int main(int argc, char* argv[]){
    //argv[1] is PORTNUM
    //argv[2] is file folder name
    char buf[BUFSIZ];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);
    char cli_file[BUFSIZ];
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd
    pid_t pid;

    int rfd, n;
    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("socket");
        exit(1);
    }

    int optvalue = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("0.0.0.0");

    if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){
        perror("bind");
        exit(1);
    }

    if(listen(sd, 5)){
        perror("listen");
        exit(1);
    }
    while(1){
        if((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
            perror("accept");
            exit(1);
        }
        switch(fork()){
            case 0:
                close(sd);
                if(recv(ns, cli_file, sizeof(cli_file), 0)==-1){
                    perror("recv\n");
                    exit(1);
                }
                sprintf(path, "%s/%s", argv[2], cli_file);
                rfd = open(path, O_RDONLY);
                if(rfd == -1){
                    perror("not open file");
                    exit(1);
                }
                while((n = read(rfd, buf, BUFSIZ))>0){
                    if(write(ns, buf, n)!=n){
                        perror("not write file");
                        exit(1);
                    }
                }
                printf("transport success '%s' file. \n",cli_file);
                close(rfd);
                exit(0);
        }
        close(ns);
    }

    return 0;
}
