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

int main(int argc, char* argv[]){
    //argv[1] is PORTNUM
    //argv[2] is file folder name
    char buf[BUFSIZ];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);
    char cli_file[BUFSIZ];
    DIR *dp;
    struct dirent *dent;
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd

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
        if((dp=opendir(argv[2]))==NULL){
            perror("not open dir");
            exit(1);
        }
        if(recv(ns, cli_file, sizeof(cli_file), 0)==-1){
            perror("recv\n");
            exit(1);
        }

        while(dent = readdir(dp)){
            if(dent->d_name[0]=='.')
                continue;
            if(!strcmp(cli_file, dent->d_name)){
                sprintf(path, "%s/%s", argv[2], dent->d_name);
                //stat(path, &file_check);
                rfd = open(path, O_RDONLY);
                if(rfd == -1){
                    perror("not open file");
                    exit(1);
                }
                while((n = read(rfd, buf, BUFSIZ))>>0){
                    if(write(ns, buf, n)!=n){
                        perror("not write file");
                        exit(1);
                    }
                }
                printf("transport success '%s' file. \n",cli_file);
                close(rfd);
            }
        }
        sprintf(buf, "Your IP address is %s", inet_ntoa(cli.sin_addr));
        if(send(ns, buf, strlen(buf)+1, 0)==-1){
            perror("send");
            exit(1);
        }
        close(ns);
    }
    close(sd);

    return 0;
}
