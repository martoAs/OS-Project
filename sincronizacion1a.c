#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

sem_t A,B,C;

void * printA(void * a){
	while(1){
		sem_wait(&A);
		sem_wait(&A);
		printf("A");
		sem_post(&B);
	}
	
	return NULL;
}

void * printB(void * a){
	while(1){
		sem_wait(&B);
		printf("B");
		sem_post(&A);
		sem_post(&C);
	}
	
	return NULL;
}

void * printC(void * a){
	while(1){
		sem_wait(&C);
		sem_post(&A);
		sem_wait(&C);
		printf("C");
		sem_post(&A);
		
		
	}
	
	return NULL;
}

int main(int argc, char **argv)
{
	sem_init(&A, 0, 2);
	sem_init(&B, 0, 0);
	sem_init(&C, 0, 0);
	
	pthread_t threadA, threadB, threadC;
	pthread_create(&threadA, NULL, printA, NULL);
    pthread_create(&threadB, NULL, printB, NULL);
    pthread_create(&threadC, NULL, printC, NULL);
    
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);
    
    sem_destroy(&A);
	sem_destroy(&B);
	sem_destroy(&C);
	
	return 0;
}

