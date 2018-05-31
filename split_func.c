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
#include<sys/stat.h>

void split(char *path, char *filename, int n)
{
	char p_name[100];
	FILE *source, *part;
	size_t parts, i;
	int p_size, w_b; b;
	struct stat status;
	

	if((source = fopen(path[], "rb")) == NULL)
	{
		perror("split fopen");
		exit(1);
	}

	stat(path, &status);

	p_size = (size_t)(status.st_size/n); // n >0
	fseek(source, 0, SEEK_END);
	
	parts = (size_t) ceil((double)ftell(source)/p_size);
	// parts = n;
	rewind(source);

	for(i=0; i<parts; i++) // split into 'parts' pieces.
	{
		printf("write parts %02d ...",i);
		sprintf(p_name, "%s.%02d", filename, i);

		if((part = fopen(p_name, "wb")) == NULL)
		{
			perror("part file open error");
			exit(1);
		}

		for(w_b=0; w_b<p_size && (b = fgetc(source)) != EOF; w_b++)
		{
			fputc(b, part);
		}
		/*
		b = fread(buf, 4096, 1, source)
		fwrite(buf, 4096, 1, part)
		change.
		*/
		printf("%lu bytes", w_b);
		fclose(part);
	}
	printf("\n end! \n");
	fclose(source);
}

