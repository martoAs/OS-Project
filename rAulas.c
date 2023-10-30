#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#define L 13
#define cHilos 25

sem_t mutex, lugares,lectores,escribir;
long int tablaReservas[L];
int lugaresLibres[L];


void elegirReserva(){
	int hora;
	int j = 0;
	
	for(int i = 0; i < L; i++){
		if(tablaReservas[i]==0){
			lugaresLibres[j] = i;
			j++;
		}
	}
	
	
	int numR = rand() % j;
	hora = lugaresLibres[numR]+9;
	tablaReservas[lugaresLibres[numR]] = pthread_self();
	//fflush(stdout);
	printf("Soy %li y reserve a la hora: %i \n",pthread_self(),hora);
	
}

void cancelar(){
	//Se asume que cancela la primera reserva que encuentra
	int hora = -1;
	int finish = 0;
	
	sem_wait(&escribir);
	for(int i = 0; i < L && !finish; i++){
		if(tablaReservas[i]==pthread_self()){
			hora = i+9;
			tablaReservas[i]=0;
			sem_post(&lugares);
			finish = 1;
            printf("Soy %li y cancele el turno de la hora %i \n",pthread_self(),hora);
		}
	
	}
    if(hora==-1){
        printf("Soy %li no tenia turnos para cancelar \n",pthread_self());
    }
	sem_post(&escribir);

	
}



void reservar(){
	sem_wait(&escribir);
	if(sem_trywait(&lugares)==0){
		
		
		elegirReserva();
		
		
	}
	else{
		//fflush(stdout);
		printf("Soy %li y no pude reservar, lugares ocupados. \n",pthread_self());
	}
	sem_post(&escribir);
	
}

void consultar(){
	
	sem_wait(&mutex);
	if(sem_trywait(&lectores)!=0){
		sem_wait(&escribir);
	}
	else{
		sem_post(&lectores);
	}
	sem_post(&lectores);
	sem_post(&mutex);
	
	for(int i = 0; i < L; i++){
		if(tablaReservas[i]==pthread_self()){
			//fflush(stdout);
			printf("Soy %li y tengo reserva a la hora %i \n",pthread_self(),i+9);
		}	
		
	}
	
	sem_wait(&lectores);
	sem_wait(&mutex);
	if(sem_trywait(&lectores)!=0){
		sem_post(&escribir);
	}
	else{
		sem_post(&lectores);
	}
	sem_post(&mutex);

}



void elegirOperacion(){
	
	
	int numA = rand() % 100 +1;
	if(numA<=50){
		reservar();
	} else if(numA<=75){
			consultar();
		}else{
				cancelar();
			}
}



void * start(void * a){
	
	int i = 0;
	while(i<4){
		elegirOperacion();
		i++;
	}

	return 0;
}



int main(int argc, char **argv)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	srand(time(NULL));
	pthread_t threads[cHilos];
    int thread_ids[cHilos];
    
    sem_init(&mutex, 0, 1);
	sem_init(&lugares, 0, 13);
	sem_init(&lectores, 0, 0);
	sem_init(&escribir,0,1);

	
	for(int j = 0; j<L; j++){
		tablaReservas[j]=0;
		lugaresLibres[j]=0;
	}
    
    for (int i = 0; i < cHilos; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, start, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < cHilos; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
	
	sem_destroy(&lectores);
	sem_destroy(&mutex);
	sem_destroy(&lugares);
	sem_destroy(&escribir);

	
	
	return 0;
}

