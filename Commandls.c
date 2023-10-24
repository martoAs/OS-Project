#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int cant, char ** args)
{
	int toReturn = 0;
	struct dirent * eDir; //Usamos el struct especial de dirent.h para poder almacenar el directorio
	
	DIR* directory = opendir("."); //Abre el directorio actual
    if (directory == NULL) {
        perror("opendir");
        toReturn=1;
    }else{
		eDir = readdir(directory);  
	    while (eDir != NULL) {
			printf("%s ", eDir->d_name);
			eDir = readdir(directory);
		}
		printf("\n");
		closedir(directory);
	}
	
	return toReturn;
}

