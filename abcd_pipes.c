       #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>



//read es como wait y write es como signal
int main(){
	int pipeAB[2];
	int pipeBA[2];
	int pipeBC[2];
	int pipeCB[2];
	int pipeCD[2];
	pid_t A, B, C, D;
	char buffer;

	
	
	if(pipe(pipeAB)==-1 || pipe(pipeBC)==-1 || pipe(pipeBA)==-1|| pipe(pipeCB)==-1|| pipe(pipeCD)==-1){
		perror("pipeAB, pipeBA, pipeCB, pipeBC or pipeCD");
		exit(1);
	}
	
	write(pipeBA[1],"B",1);
	
	
	
	A = fork();
	if(A==-1){
		perror("ERROR CREATING CHILD A");
		exit(1);
	}
	else if(A==0){
		close(pipeAB[0]); //Cierra el extremo de lectura
		close(pipeBA[1]); //Cierra el extremo de escritura
		
		while(1){
			
			read(pipeBA[0], &buffer, 1);
			if(buffer=='B'){
				printf("A");
				fflush(stdout);
				write(pipeAB[1], "A", 1);
				usleep(100000);
			}
		}
		exit(0);
	}
			
		
		
	
	
	B = fork();
	if(B==-1){
		perror("ERROR CREATING CHILD B");
		exit(1);
	}
	else if(B==0){
		close(pipeCB[1]); //Cierra el extremo de esc
		close(pipeAB[1]); //Cierra el extremo de escritura
		close(pipeBC[0]);
		close(pipeBA[0]); //Cierra el extremo de lect

		while(1){
				
			read(pipeAB[0], &buffer, 1); 
			if(buffer=='A'){
			    printf("B");
				fflush(stdout);
				write(pipeBC[1], "B", 1);
				read(pipeCB[0], &buffer, 1);
				if (buffer == 'C'){
					write(pipeBA[1],"B",1);
				}
			}

            }
 
		
		exit(0);
	}
	
	C = fork();
	if(C==-1){
		perror("ERROR CREATING CHILD C");
		exit(1);
	}
	else if(C==0){
		
		close(pipeBC[1]); //escritura
		close(pipeCB[0]); //lect
		close(pipeCD[0]);
		while(1){

			read(pipeBC[0],&buffer,1);
			if(buffer=='B'){
				write(pipeCB[1], "C", 1);
				read(pipeBC[0],&buffer,1);
				if(buffer=='B'){
					printf("C");
					fflush(stdout);
					write(pipeCB[1],"C",1);
					read(pipeBC[0],&buffer,1);
					if(buffer=='B'){
						printf("C");
						fflush(stdout);
						write(pipeCD[1],"C",1);
					}
				}
			}
		}
		
		exit(0);
	}
	
	D = fork();
	if(D==-1){
		perror("ERROR CREATING CHILD C");
		exit(1);
	}
	else if(D==0){
		close(pipeCD[1]);
		close(pipeCB[0]);
		while(1){

			read(pipeCD[0],&buffer,1);
			if(buffer=='C'){
					printf("D");
					fflush(stdout);
					write(pipeCB[1],"C",1);
				}
			}
			exit(0);
		}
		

	
	close(pipeAB[0]); 
    close(pipeAB[1]); 
    close(pipeBC[0]);
    close(pipeBC[1]);
    close(pipeBA[0]);
    close(pipeBA[1]);
    close(pipeCB[0]);
    close(pipeCB[1]);
    close(pipeCD[0]);
    close(pipeCD[1]);

	
	wait(NULL);
	wait(NULL);
	wait(NULL);
	
	
	
	
	return 0;
}
