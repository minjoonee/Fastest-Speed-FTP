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
    char buf[BUF]="";
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);
    char cli_file[BUFSIZ];
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd
    DIR *dp;
    struct dirent *dent;
    pid_t pid;
    int rfd, n;
    int i =0;
    int optvalue = 1;
    FILE *rfp[7];
    char file_name[7][BUFSIZ];

    if((dp = opendir(argv[2]))==NULL){
        perror("directory not open");
        exit(1);
    }
    while(dent = readdir(dp)){
        sprintf(path, "%s/%s", argv[2], dent->d_name);
        if(dent->d_name[0]=='.') continue;
        else{
            rfp[i]=fopen(path, "r");
            while((n=fread(buf, sizeof(char),sizeof(BUF),rfp[i]))>0);
            fseek(rfp[i],1,SEEK_SET);
            printf("complete read %s\n",path);
            strcpy(file_name[i], dent->d_name);
            i++;
        }
    }
    printf("complete memory allocate\n");
    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("socket");
        exit(1);
    }

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
                sprintf(path, "%s/%s",argv[2],cli_file);
                printf("input client %s\n",path);
                for(i=0; i<7; i++)
                    if(!strcmp(file_name[i],cli_file))
                        break;
                while((n=fread(buf,sizeof(char),sizeof(BUF),rfp[i]))>0){
                    send(ns, buf, n, 0);
                }
                printf("transport success '%s' file. \n",cli_file);
                fseek(rfp[i],1,SEEK_SET);
                exit(0);
        }
        close(ns);
    }
    for(i=0; i<7; i++)
        fclose(rfp[i]);

    return 0;
}
