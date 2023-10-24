#include <stdio.h>
#include <unistd.h>
//#include <sys/wait.h>
//#include <sys/types.h>
//#include <sys/stat.h>

int main(int cant, char ** args){
		
	int toReturn = 0;
	char * directory = args[1];
	char currentDirectory[1024]; //Se asume que un directorio no va a ocupar mas de 1024 lineas
	
	if(chdir(directory) == 0){
		getcwd(currentDirectory, sizeof(currentDirectory));
		printf("Changed directory to: %s\n", currentDirectory);
	}
	else{
		perror("cd");
		toReturn=1;
	}
	
	return toReturn;
}
