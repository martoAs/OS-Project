#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define cLugares 13
#define cProcesos 25
#define KEY ((key_t) (1243))
#define SEGSIZE sizeof (struct sharedMemory)

struct sharedMemory{
    sem_t mutex, lugares,lectores,escribir;
    long int tablaReservas[cLugares];
    int lugaresLibres[cLugares];
};
//-------------------------------------------------------------------------------//

void elegirReserva(struct sharedMemory *mem, int id){
    int hora;
    int j = 0;

    for(int i = 0; i < cLugares; i++){
        if(mem->tablaReservas[i]==0){
            mem->lugaresLibres[j] = i;
            j++;
        }
    }

    int numR = rand() % j;
    hora = mem->lugaresLibres[numR]+9;
    mem->tablaReservas[mem->lugaresLibres[numR]] = id;
    printf("Soy %i y reserve a la hora: %i \n",id,hora);
}
void reservar(struct sharedMemory *mem, int id){
    sem_wait(&mem->escribir);

    if(sem_trywait(&mem->lugares)==0){
        elegirReserva(mem,id);
    }else{
        printf("Soy %i y no pude reservar \n",id);
    }
    sem_post(&mem->escribir);
}
void consultar(struct sharedMemory *mem, int id){
    sem_wait(&mem->mutex);
    if(sem_trywait(&mem->lectores)!=0){
        sem_wait(&mem->escribir);
    }
    else{
        sem_post(&mem->lectores);
    }
    sem_post(&mem->lectores);
    sem_post(&mem->mutex);
    printf("Soy %i y quiero consultar \n",id);
    for(int i = 0; i < cLugares; i++){
        if(mem->tablaReservas[i]==id){
            printf("Soy %i y tengo reserva a la hora %i \n",id,i+9);
        }
    }

    sem_wait(&mem->lectores);
    sem_wait(&mem->mutex);
    if(sem_trywait(&mem->lectores)!=0){
        sem_post(&mem->escribir);
    }
    else{
        sem_post(&mem->lectores);
    }
    sem_post(&mem->mutex);
}
void cancelar(struct sharedMemory *mem, int id){
    int hora = -1;
    int finish = 0;
    sem_wait(&mem->escribir);

    for(int i = 0; i < cLugares && !finish; i++){
        if(mem->tablaReservas[i]==id){
            hora = i+9;
            mem->tablaReservas[i] = 0;
            sem_post(&mem->lugares);
            finish=1;
            printf("Soy %i y cancele mi reserva a la hora %i \n",id,hora);
        }
    }
    if(hora==-1){
        printf("Soy %i no tenia turnos para cancelar \n",id);
    }
    sem_post(&mem->escribir);
}
void elegirOperacion(struct sharedMemory *mem,int id){
    int numA = rand() % 100 +1;
    if(numA<=50){
        reservar(mem,id);
    } else if(numA<=75){
        consultar(mem,id);
    }else{
        cancelar(mem,id);
    }
}
void start(){
    int i = 0;
    int pid = getpid();
    srand(time(NULL)+pid);
    int id = shmget(KEY, SEGSIZE, 0);
    if(id<0){
        perror("Fallo en shmget hijo");
        exit(1);
    }
    //Asocio la memoria a la estructura local
    struct sharedMemory *mem = (struct sharedMemory *) shmat(id, 0, 0);
    if (mem <= (struct sharedMemory *) (0)){
        printf(" Error fallo shmat hijo \n"); exit(2);
    }

    while(i<4){
        elegirOperacion(mem,pid);
        i++;
    }
}


int main(){
    setvbuf(stdout, NULL, _IONBF, 0);
    //Creacion de la memoria compartida
    struct sharedMemory *mem;
    int shmid = shmget(KEY, SEGSIZE, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("Fallo en shmget");
        exit(1);
    }
    //Asociamos el segmento de memoria compartida
    mem = (struct sharedMemory *) shmat(shmid, 0, 0);
    if (mem <= (struct sharedMemory *) (0)){
        printf(" Error fallo shmat \n"); exit(2);
    }
    sem_init(&mem->mutex,1,1);
    sem_init(&mem->lugares,1,13);
    sem_init(&mem->lectores,1,0);
    sem_init(&mem->escribir,1,1);
    for(int j = 0; j < cLugares; j++){
        mem->tablaReservas[j] = 0;
        mem->lugaresLibres[j] = 0;
    }
    //Creacion de los procesos
    int i = 0;
    pid_t p;
    while(i < cProcesos){
        p = fork();
        if(p==0){
            start();
            exit(0);
        }
        else if(p<0){
            perror("Fallo en fork");
            exit(1);
        }

        i++;
    }
    //Espera de los procesos
    while(i > 0){
        wait(NULL);
        i--;
    }
    //Liberacion de la memoria compartida
    if(shmdt((char *) mem) == -1){
        perror("Error en shmdt");
        exit(1);
    }
    if(shmctl(shmid, 0, 0) == -1){
        perror("Error en shmctl");
        exit(1);
    }



    return 0;
}

