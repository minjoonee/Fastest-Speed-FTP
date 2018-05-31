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
#include<math.h> // ceil() 
// 일단 지금 쓰는 헤더파일 다 해놓음.

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

	stat(path, &status);	// 스탯 불러옴.
	
	p_size = (size_t)ceil(status.st_size/n); // n > 0, 전체 크기를 n 분할한 크기
	fseek(source, 0, SEEK_END);
	
	parts = (size_t) ceil((double)ftell(source)/p_size);	// 필요 없는 부분, parts = n; 이렇게 대체할 수 있음.
	
	// 아직 수정한거 테스트 안해봄.
	
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
		1바이트씩 읽고 있는데 4k 씩 읽도록 변경해야함. 아직 바꿔서 테스트 안해봄.
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

