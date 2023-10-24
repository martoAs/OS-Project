#include <stdio.h>

int main(int cant, char ** args){
	
	FILE * displayFile = fopen(args[1],"r");
	char line[256]; //Se asume que cada linea no va a tener mas de 256 caracteres
	int toReturn = 0;
		
	if(displayFile == NULL){ //Manejo de error ante que no se haya podido abrir el archivo
		perror("Unable to open the requested file");
		toReturn = 1;
	}else{
	
		while(fgets(line,sizeof(line),displayFile)!=NULL){ //Imprime las lineas del archivo 
			printf("%s \n",line);
		}
	
		fclose(displayFile);	
	}
	return toReturn;
}
