#include <stdio.h>

int main(int cant, char ** args){
	
	FILE * helpFile = fopen("helpD.txt","r");
	char line[256]; //Se asume que cada linea no va a tener mas de 256 caracteres
	int toReturn = 0;
		
	if(helpFile == NULL){ //Manejo de error ante que no se haya podido abrir el archivo
		
		perror("Unable to open the requested file");
		toReturn = 1;
	}else{
	
	while(fgets(line,sizeof(line),helpFile)!=NULL){ //Imprime las lineas del archivo donde esta el detalle de los comandos soportados por la mini shell
		printf("%s \n",line);
	}
	
	fclose(helpFile);	
}
	return toReturn;
}
