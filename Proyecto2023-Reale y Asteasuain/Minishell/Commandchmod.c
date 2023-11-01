#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int cant, char ** args)
{
	int toReturn = 0;
	char * fileName = args[2];
	char * newP = args[1];
	char * endS; //Esta variable se usara para confirmar que los nuevos permisos esten en octal para la funcion strtol
	
	if(newP!=NULL){
		strtol(newP,&endS,8); //chequea que newP (los permisos) esten en octal
	
		if(*endS=='\0'){ //Exito
			if(fileName!=NULL){
				
				mode_t newPermissions = (mode_t)strtol(newP, NULL, 8);

				if (chmod(fileName, newPermissions) == 0) {
					printf("Permissions for %s changed successfully\n", fileName);
				} else {
					perror("chmod");
					toReturn=1;
				}
			}
			else{
				printf("Must provide a file. Try again \n");
			}
		}else{
			printf("The new permissions are not in octal format. Try again \n");
		}
	}else{
		printf("Must provide a number. Try again \n");
	}
	
	

	
	return toReturn;
}

