#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(){
	int pipeAB[2]; //Pipe conexion A->B
	int pipeBC[2]; //Pipe conexion B->C
	int pipeCBA[2]; //Pipe conexion B->A y C->A
	pid_t A, B, C; //Los tres procesos encargados de imprimir los caracteres
	char buffer;

	
	
	if(pipe(pipeAB)==-1 || pipe(pipeBC)==-1 || pipe(pipeCBA)==-1){
		perror("pipeAB,pipeBA or pipeCBA");
		exit(1);
	}
	
	write(pipeCBA[1],"C",1);
	write(pipeCBA[1],"C",1); //Se empieza escribiendo en el buffer dos C para activar a A
	
	
	A = fork();
	if(A==-1){
		perror("ERROR CREATING CHILD A");
		exit(1);
	}
	else if(A==0){ //Empieza el codigo correspondiente al proceso encargado de imprimir el caracter A
		close(pipeAB[0]); //Cierra el extremo de lectura
		close(pipeCBA[1]); //Cierra el extremo de escritura
		close(pipeBC[1]); close(pipeBC[0]); //Cierra el pipe B->C 
		
		while(1){
			
			read(pipeCBA[0], &buffer, 1);
			if(buffer=='C' || buffer=='B'){
				read(pipeCBA[0], &buffer, 1);
				if(buffer=='B' || buffer=='C'){
					printf("A");
					fflush(stdout);
					write(pipeAB[1], "A", 1);
					usleep(100000); 
				}
			}
		}
			exit(0);
	}
		
		
	
	
	B = fork();
	if(B==-1){
		perror("ERROR CREATING CHILD B");
		exit(1);
	}
	else if(B==0){//Empieza la ejecucion del proceso encargado de imprimir el caracter B
		close(pipeCBA[0]); //Cierra el extremo de lectura
		close(pipeAB[1]); //Cierra el extremo de escritura
		close(pipeBC[0]);
		while(1){
				
			read(pipeAB[0], &buffer, 1); 
			if(buffer=='A'){
			    printf("B");
				fflush(stdout);
				write(pipeCBA[1], "B", 1);
				write(pipeBC[1],"B",1);
			}

            }
 
		
		exit(0);
	}
	
	C = fork();
	if(C==-1){
		perror("ERROR CREATING CHILD C");
		exit(1);
	}
	else if(C==0){ //Empieza la ejecucion del proceso encargado de imprimir el caracter C
		
		close(pipeBC[1]); //Cierra escritura
		close(pipeCBA[0]); //Cierra lectura
		close(pipeAB[0]); close(pipeAB[1]); //Cierra pipe A->B 
		
		while(1){

			read(pipeBC[0],&buffer,1);
			if(buffer=='B'){
				write(pipeCBA[1], "B", 1);
				read(pipeBC[0],&buffer,1);
				if(buffer=='B'){
					printf("C");
					fflush(stdout);
					write(pipeCBA[1],"C",1);
				}
			}
		}
		
		exit(0);
	}
	
	close(pipeAB[0]); 
    close(pipeAB[1]); 
    close(pipeBC[0]);
    close(pipeBC[1]);
    close(pipeCBA[0]);
    close(pipeCBA[1]);

	
	wait(NULL);
	wait(NULL);
	wait(NULL);
	
	
	
	
	return 0;
}

