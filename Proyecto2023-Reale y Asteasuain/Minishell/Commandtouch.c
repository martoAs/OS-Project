#include <stdio.h>

int main(int cant, char ** args){

	int toReturn=0;
	
	FILE * newFile = fopen(args[1],"w");
	if(newFile==NULL){
		perror("touch");
		toReturn=1;
	}else{
		printf("File created successfully \n");
		fclose(newFile);
	}
	
	
	return toReturn;
	
}
