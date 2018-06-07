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
#include<sys/wait.h>
#define BUF 4096

pid_t wait(int *stat_loc);
int main(int argc, char* argv[]){
    //argv[1] is PORTNUM
    //argv[2] is file folder name
    char buf[BUF];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);
    int PORTNUM = atoi(argv[1]);	// port number
    char cli_file[BUFSIZ];
    struct stat file_check; // file extension check
    char path[BUFSIZ]; // file pwd
    int status;
    DIR *dp;
    struct dirent *dent;
    char zip_filename[BUFSIZ];
    int rfd[7], n, f;
    char fn[7][BUFSIZ];

	chdir(argv[2]);	//Move the working directory
    if((dp=opendir("."))==NULL) printf("not open dir\n");	// open directory
    while(dent=readdir(dp)){	// read directory
        if(!strcmp("2.fna",dent->d_name)||!strcmp("kernel.tar", dent->d_name)){// if it is fna or tar file
            sprintf(path, "%s",dent->d_name);	// Specify the path
            sprintf(zip_filename,"%s.zip",dent->d_name);	// Naming compressed file
            switch(fork()){	
                case 0:
                    if(execlp("zip","zip",zip_filename,path,(char *)NULL)==-1) //Compress using execlp(zip)
                        perror("execlp");
            }
        }
    }
    closedir(dp);	// close directory
    printf("zip......\n");
    while(wait(&status) > 0);	// wait until compression is complete


    f=0;
    if((dp=opendir("."))==NULL) printf("not open dir\n");	// open directory again
    while(dent=readdir(dp)){	// read directory again
        if(dent->d_name[0]=='.'||!strcmp("2.fna",dent->d_name)||!strcmp("kernel.tar",dent->d_name)) continue;
		// if it is . .. 2.fna kernal.tar , go to the next.
        else{
            sprintf(path, "%s", dent->d_name);	// Spetify the path
            rfd[f]=open(path, O_RDONLY);	// file open
            while((n=read(rfd[f], buf, BUF))>0);	// read file.
            strcpy(fn[f], dent->d_name);	// Save the file name.
            f++;
            printf("%s\n", path);	// Check the read file
        }
    }
	closedir(dp);	// close directory.

    printf("total read\n");

    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){	//Create socket descriptor 
        perror("socket");
        exit(1);
    }

    int optvalue = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));

	// Initialize socket.
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
            perror("accept"); // Miss the accept , print error message.
            exit(1);
        }
		// multi process - Because open files should not be shared with each other.
        switch(fork()){
            case 0: // child process
                close(sd);	// close the server socket
                if(recv(ns, cli_file, sizeof(cli_file), 0)==-1){	// receive the filename from client.
                    perror("recv\n");
                    exit(1);
                }
                for(f=0; f<7; f++)	// Search filename.
                    if(!strcmp(cli_file, fn[f]))
                        break;
                lseek(rfd[f],0,SEEK_SET); // Move the offset to the beginning of the file.
                while((n = read(rfd[f], buf, BUF))>0)	// read file
                    send(ns, buf, n, 0);	// send file
				send(ns, NULL, 0,0); // Send end signal.
                printf("transport success '%s' file. \n", cli_file);
                exit(0);	// end child process
        }
        close(ns);	// close the client socket.
    }

    return 0;
}
