#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t mesaEntrada, escribir;

sem_t colaClienteComun, hayClienteEsperando, clienteComunAtendido, despertarEmpleadoComun; //Asociado a Cliente comun.

sem_t colaClienteEmpresa, hayEmpresarioEsperando, empresarioAtendido, despertarEmpleadoEmpresa;

sem_t colaClientePolitico, hayPoliticoEsperando, politicoAtendido;


void atenderPolitico(int n){
    sem_wait(&escribir);
    printf("empleado %d esta atendiendo a un politico\n",n);
    sem_post(&escribir);
}
void atenderEmpresa(int n){
    sem_wait(&escribir);
    printf("empleado %d esta atendiendo a una empresa\n",n);
    sem_post(&escribir);
}
void atenderComun(int n){
    sem_wait(&escribir);
    printf("empleado %d esta atendiendo a un cliente comun\n",n);
    sem_post(&escribir);
}

void* metodoEmpleadoComun(){
    while(1){
        sem_wait(&despertarEmpleadoComun);
        if (sem_trywait(&hayPoliticoEsperando) == 0){
            atenderPolitico(3);
            sem_post(&politicoAtendido);
        }
        else{
            if (sem_trywait(&hayClienteEsperando) == 0){ //Empleado3 espera a que tenga Usuarios comunes para atender.
                atenderComun(3);//Esta atiendiendo
                sem_post(&clienteComunAtendido);
            }
        }
    }

}

void* metodoEmpleadoEmpresa1(){
    while(1){
        sem_wait(&despertarEmpleadoEmpresa);
        if (sem_trywait(&hayPoliticoEsperando) == 0){
            atenderPolitico(1);
            sem_post(&politicoAtendido);
        }
        else{
            if (sem_trywait(&hayEmpresarioEsperando) == 0){
                atenderEmpresa(1);// Esta atendiendo.
                sem_post(&empresarioAtendido);
            }
        }
    }
}
void* metodoEmpleadoEmpresa2(){
    while(1){
        sem_wait(&despertarEmpleadoEmpresa);
        if (sem_trywait(&hayPoliticoEsperando) == 0){
            atenderPolitico(2);
            sem_post(&politicoAtendido);
        }
        else{
            if (sem_trywait(&hayEmpresarioEsperando) == 0){
                atenderEmpresa(2);// Esta atendiendo.
                sem_post(&empresarioAtendido);// Espera hata que el empresario decida terminar su consulta con el empleado.
            }
        }
    }
}

void *clienteComun(){
    if (sem_trywait(&mesaEntrada) == 0) {//Cliente intenta entrar al Banco, si hay lugar en la cola ingresa

        sem_wait(&escribir);
        printf("Cliente Comun se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        sem_post(&escribir);

        //El cliente entro al banco

        sem_wait(&colaClienteComun);

        sem_wait(&escribir);
        printf("cliente comun ingreso a cola de clientes comunes\n");
        sem_post(&escribir);

        sem_post(&mesaEntrada);//libera cola para mesa de entrada cuando ingresa a colaClienteComun

        sem_post(&hayClienteEsperando); // avisa que hay un nuevo cliente esperando a ser atendido
        sem_post(&despertarEmpleadoComun);

        sem_wait(&clienteComunAtendido);
        //Una vez es atendido, libera la colaCliente comun :
        sem_post(&colaClienteComun);
    }else
        printf("La cola está llena, el cliente se va del banco\n");
    return NULL;
}


void *clienteEmpresa(){
    if (sem_trywait(&mesaEntrada) == 0) {//Cliente intenta entrar al Banco, si hay lugar en la cola ingresa

        sem_wait(&escribir);
        printf("Cliente Empresa se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        sem_post(&escribir);

        //El empresario entro al banco

        sem_wait(&colaClienteEmpresa);// Espera en cola mesa entrada hasta que haya lugar para  entrar en cola de empresarios.

        sem_wait(&escribir);
        printf("Empresario ingreso a cola de Empresarios\n");
        sem_post(&escribir);
        sem_post(&mesaEntrada);//libera cola para mesa de entrada cuando ingresa a colaClienteComun

        //Espera a ser atendido por algun empleado:
        sem_post(&hayEmpresarioEsperando);//avisa que hay un nuevo empresario esperando a ser atendido.

        //despierta a los dos empleados que pueden atenderlo
        sem_post(&despertarEmpleadoEmpresa);
        sem_post(&despertarEmpleadoEmpresa);

        sem_wait(&empresarioAtendido);//espera a terminar de ser atendido por algun empleado
        sem_post(&colaClienteEmpresa);//una vez que fue atendido libera la cola de empresarios para que otro cliente pueda ingresar en ella
    }
    else
        printf("La cola está llena, el cliente se va del banco\n");
    return NULL;
}


void *clientePolitico(){
    if (sem_trywait(&mesaEntrada) == 0) {//Cliente intenta entrar al Banco
        sem_wait(&escribir);
        printf("Cliente politico se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        sem_post(&escribir);

        //El politico entro al banco

        sem_wait(&colaClientePolitico);// Espera en cola mesa entrada hasta entrar en cola propia de politicos.
        sem_wait(&escribir);
        printf("Politico ingreso a cola de Politicos\n");
        sem_post(&escribir);
        sem_post(&mesaEntrada);//libera cola para mesa de entrada cuando ingresa a colaClienteComun

        sem_post(&hayPoliticoEsperando); //avisa que hay un nuevo politico esperando a ser atendido

        //despierta a los 3 empleados, para que si algunoi esta libre lo atienda con prioridad
        sem_post(&despertarEmpleadoEmpresa);
        sem_post(&despertarEmpleadoEmpresa);
        sem_post(&despertarEmpleadoComun);

        sem_wait(&politicoAtendido); //espera a ser atendido por completo para liberar la cola de politicos

        sem_post(&colaClientePolitico);

    }
    else
        printf("La cola está llena, el cliente se va del banco\n");
    return NULL;
}


int main() {

    sem_init(&mesaEntrada, 0, 30);
    sem_init(&escribir, 0, 1);


    //Semaforos para Cliente comun:
    sem_init(&colaClienteComun, 0, 15);
    sem_init(&hayClienteEsperando,0,0);
    sem_init(&despertarEmpleadoComun, 0, 0);
    sem_init(&clienteComunAtendido,0,0);

    //Semaforos para Empresario:
    sem_init(&colaClienteEmpresa, 0, 15);
    sem_init(&hayEmpresarioEsperando, 0, 0);
    sem_init(&despertarEmpleadoEmpresa,0,0);
    sem_init(&empresarioAtendido,0,0);

    //semaforos politico
    sem_init(&hayPoliticoEsperando, 0, 0);
    sem_init(&colaClientePolitico, 0, 15);
    sem_init(&politicoAtendido,0,0);



    //Creacion de hilos para empleados, politicos, empresarios y clientes comunes:
    pthread_t hiloEmpleadoComun;
    pthread_create(&hiloEmpleadoComun, NULL, metodoEmpleadoComun, NULL);

    pthread_t hiloEmpleadoEmpresa1;
    pthread_create(&hiloEmpleadoEmpresa1, NULL, metodoEmpleadoEmpresa1, NULL);

    pthread_t hiloEmpleadoEmpresa2;
    pthread_create(&hiloEmpleadoEmpresa2, NULL, metodoEmpleadoEmpresa2, NULL);

    pthread_t threadsClientesComunes[45];
    for(int i = 0; i < 10; i++) {
        pthread_create(&threadsClientesComunes[i], NULL, clienteComun, NULL);
    }

    pthread_t threadsClientesEmpresa[45];
    for(int i = 0; i < 10; i++) {
        pthread_create(&threadsClientesEmpresa[i], NULL, clienteEmpresa, NULL);
    }

    pthread_t threadsClientesPoliticos[45];
    for(int i = 0; i < 10; i++) {
        pthread_create(&threadsClientesPoliticos[i], NULL, clientePolitico, NULL);
    }

    for(int i = 0; i < 10; i++) {
        pthread_join(threadsClientesComunes[i], NULL);
    }

    for(int i = 0; i < 10; i++) {
        pthread_join(threadsClientesPoliticos[i], NULL);
    }

    for(int i = 0; i < 10; i++) {
        pthread_join(threadsClientesEmpresa[i], NULL);
    }

    sem_destroy(&mesaEntrada);
    sem_destroy(&escribir);


    sem_destroy(&colaClienteComun);
    sem_destroy(&colaClienteEmpresa);
    sem_destroy(&colaClientePolitico);

    sem_destroy(&hayClienteEsperando);
    sem_destroy(&hayEmpresarioEsperando);
    sem_destroy(&hayPoliticoEsperando);

    sem_destroy(&despertarEmpleadoComun);
    sem_destroy(&despertarEmpleadoEmpresa);

    sem_destroy(&politicoAtendido);
    sem_destroy(&empresarioAtendido);
    sem_destroy(&clienteComunAtendido);

    return 0;
}