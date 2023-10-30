#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    //--DECLARACION DE VARIABLES--
    
    char input[1024]; //Se asume que el input no va a tener mas de 1024 caracteres
    //char current_dir[1024]; // Guarda el directorio actual
    
    const char* valid_commands[] = {"mkdir","exit","help","rmdir","touch","cat","ls","chmod"}; //Comandos soportados por la mini shell
    const int num_valid_commands = sizeof(valid_commands) / sizeof(valid_commands[0]);
    char* tokens[20]; // Asumimos un maximo de 20 tokens
    char* token;
    int tokenCount;
    
 
    
    int finish = 0;

    
    
    //--COMIENZA EL CODIGO DE LA MINI SHELL--
    
    
    while (!finish) {
        char command[20]= "Command";
       
        printf("miniShell $ ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            finish = 1;
        } else{

            // Remueve el final de linea y lo cambia por un final de cadena asi funcionan las funciones de libreria string
            input[strcspn(input, "\n")] = '\0';

            // Dividimos el input dado en tokens

            token = strtok(input, " ");
            tokenCount = 0;
            while (token != NULL && tokenCount < 20) {
                tokens[tokenCount++] = token;
                token = strtok(NULL, " ");
            }
            tokens[tokenCount] = NULL; // terminamos la lista con un nulo para indicar su fin

            if (tokenCount > 0) {
                int is_valid_command = 0;

                // Chequea si el comando ingresado es valido
                for (int i = 0; i < num_valid_commands && !is_valid_command; i++) {
                    if (strcmp(tokens[0], valid_commands[i]) == 0) {

                        is_valid_command = 1;
                    }
                }

                if (is_valid_command) {

                    if(strcmp(tokens[0],"exit")==0){
                        printf("Goodbye! Have a nice day :)");
                        finish = 1;
                    }else{
                        int childpid = fork();

                        if(childpid==0){

                            execv(strcat(command,tokens[0]),tokens);

                        }else if (childpid<0) {
                            perror("fork");
                            exit(1);
                        }
                        else{
                            waitpid(childpid,NULL,0);
                        }
                    }
                } else {
                    printf("Invalid command: %s\n", tokens[0]);
                }
            }

        }

    }
    
    return 0;
}
