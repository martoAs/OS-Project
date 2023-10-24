#include <stdio.h>
#include <unistd.h>


int main(int cant, char ** args){
		
	int toReturn = 0;
	char * directory = args[1];
	
	if(directory!=NULL){ //Se controla que el usuario haya ingresado un nombre para eliminar el directorio
		if(rmdir(directory)==0)
			printf("Directory %s deleted successfully :D \n",directory);
		else {
			perror("rmdir");
			toReturn=1;
		}
	}
	
	return toReturn;

}
