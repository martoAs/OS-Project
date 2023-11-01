#include <stdio.h>
#include <sys/stat.h>

int main(int cant, char ** args){
	
	int toReturn = 0;
	char * directory = args[1];
	
	if(directory!=NULL){ //Se controla que el usuario haya ingresado un nombre para el nuevo directorio
		if(mkdir(directory,0777)==0)
			printf("Directory %s created successfully :D \n",directory);
		else {
			perror("mkdir");
			toReturn=1;
		}
	}

	return toReturn;
}
