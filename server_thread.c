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

typedef struct data{
	int *ns;
	int rfd[30];
	char fn[30][BUFSIZ];
}data;

void Split(char *path, char *filename, int n);
void *threadA(void *data);

int f = 0;

int main(int argc, char* argv[]){
    //argv[1] is PORTNUM
    //argv[2] is file folder name
    char buf[BUF];
    struct sockaddr_in sin, cli;
    int sd, *ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);
    char cli_file[BUFSIZ];
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd
	char path2[BUFSIZ];
    DIR *dp, *dp2;
    struct dirent *dent;

    int n; 
	data *dat = malloc(sizeof(data));
	pthread_t tid;

	if(mkdir("part", 0755) == -1)
	{
	//	perror("mkdir error");
	//	exit(1);
	}	
/*
    if((dp=opendir(argv[2]))==NULL) printf("not open dir\n");

	while(dent=readdir(dp))
	{
		if(dent->d_name[0] == '.') continue;

		sprintf(path2, "part/%s", dent->d_name);
		sprintf(path, "%s/%s", argv[2], dent->d_name);
		Split(path, path2, 4);
		printf("split %s\n", dent->d_name);
	}
	closedir(dp);
 alread exists
 */
	printf("split complete\n");

	if((dp2 = opendir("part")) == NULL) printf("not open dir2\n");

    while(dent=readdir(dp2)){
        if(dent->d_name[0]=='.') continue;
        else{
            sprintf(path, "part/%s", dent->d_name);
            dat->rfd[f]=open(path, O_RDONLY);
            while((n=read(dat->rfd[f], buf, BUF))>0);
            strcpy(dat->fn[f], dent->d_name);
            f++;
            printf("%s\n", path);
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

    if(listen(sd, 10)){
        perror("listen");
        exit(1);
    }
    while(1){
		ns = malloc(sizeof(int));
        if((*ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
            perror("accept");
            exit(1);
        }
		dat->ns = ns;

		pthread_create(&tid, NULL, threadA, (void *)dat);
		
		
    }

    return 0;
}

void *threadA(void *dat)
{
	data *data = (void *)dat;
	int *ns;
	ns = data->ns;
	char path[BUFSIZ];
	char cli_file[BUFSIZ];
	char nbuf[BUF];
	int n;
    if(recv(*ns, cli_file, sizeof(cli_file), 0)==-1){
     	perror("recv\n");
        exit(1);
    } 
	for(f=0; f<24; f++)                    
		 if(!strcmp(cli_file, data->fn[f]))
			 break;
	sprintf(path, "part/%s", cli_file);
	lseek(data->rfd[f],0,SEEK_SET);
	while((n = read(data->rfd[f], nbuf, BUF))>0)
	{
		 if(write(*ns, nbuf, n)!=n){
			 perror("not write file");
			 exit(1);
		 }
	}
	printf("transport success '%s' file. \n",cli_file);
	
	close(*ns);
	free(ns);
}
