#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h> 

#define MAX_NUM 100000

int main(int argc,char *argv[]){
	int count;
	int remain;
	int random;
	int max_num;
	char* buffer = (char*)malloc(12);
	char* filename = (char*)malloc(24);
	memset(buffer,0,12);
	memset(filename,0,24);
	
	srand((unsigned)time(NULL));
	
	if(argc == 2){
		max_num = atoi(argv[1]);
		strcpy(filename, "resource_");
		strcat(filename,argv[1]);
		strcat(filename,"\0");
		remain = max_num;
	}else{
		strncpy(filename, "resource_10",24);
		max_num = 10;
		remain = max_num;
	}
	
	
	FILE* fp = fopen(filename, "w");
	
	for(count = 0; count < max_num; count++){
		random = rand()%(4*1024*1024)+1;
		sprintf(buffer,"+ %d\n",random);
		//strcat(buffer,"\n");
		fwrite(buffer,strlen(buffer),1,fp);
		memset(buffer,0,12);
		
		if(random%2 == 0){
			fwrite("-\n",2,1,fp);
			remain--;
		}
	}
	
	for(count = 0; count < remain; count++){
		fwrite("-\n",2,1,fp);
	}
	
	fclose(fp);
	
	
}