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
#include<string.h>
#include<pthread.h>
#define BUF 4096
#define SPLIT_NUM 4

void Split(char *path, char *filename, int n);
// path : pwd file, filename : make file01,02,03...., n : split number
void *thread(void *arg);
int rfd[30];
char fn[30][BUFSIZ];

int main(int argc, char* argv[]){
    //argv[1] is PORTNUM
    //argv[2] is file folder name
    char buf[BUF];
    struct sockaddr_in sin, cli;
    int sd, *ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);
    char cli_file[BUFSIZ];
    char split_file[BUFSIZ];
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd
    pid_t pid;
    DIR *dp;
    struct dirent *dent;
    pthread_t tid;
    int n, f, i;

    if ((dp=opendir(argv[2]))==NULL) printf("not open dir\n");
    while(dent=readdir(dp)){
        if(dent->d_name[0]=='.') continue;
        else{
            printf("splitting %s...\n", path);
            sprintf(path, "%s/%s",argv[2], dent->d_name);
            Split(path, dent->d_name, SPLIT_NUM);
        }
    }
    printf("complete split files\n");
    closedir(dp);

    f=0;
    if((dp=opendir("split_files"))==NULL) printf("not open dir\n");
    while(dent=readdir(dp)){
        if(dent->d_name[0]=='.') continue;
        else{
            sprintf(path, "split_files/%s", dent->d_name);
            rfd[f]=open(path, O_RDONLY);
            while((n=read(rfd[f], buf, BUF))>0);
            strcpy(fn[f], dent->d_name);
            printf("read %s\n", fn[f]);
            f++;
        }
    }
    printf("total read\n");

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
        ns = malloc(sizeof(int));
        if((*ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
            perror("accept");
            exit(1);
        }
        pthread_create(&tid, NULL, thread, (void *)ns);
        pthread_detach(tid);
        //close(ns);
    }

    return 0;
}
void *thread(void *arg){
    int ans = *(int *)arg;
    char path[BUFSIZ];
    char cli_file[BUFSIZ];
    char nbuf[BUF];
    int n, f;
    if(read(ans, cli_file, sizeof(cli_file))==-1){
        perror("recv\n");
        return NULL;
    }
    for(f=0; f<28; f++)
        if(!strcmp(cli_file, fn[f]))
            break;
    sprintf(path, "split_files/%s",cli_file);
    lseek(rfd[f],0, SEEK_SET);
    printf("transport '%s' file\n",cli_file);
    while((n=read(rfd[f], nbuf, BUF))>0)
        if(write(ans, nbuf, n)!=n){
            perror("not write file");
            return NULL;
        }
    printf("transprot success '%s' file \n", cli_file);
    close(ans);
    free(arg);
    return NULL;
}
