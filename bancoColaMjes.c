#include "mensaje.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int colaClienteComun, colaClienteEmpresa, colaClientePolitico, colaMesaEntrada, politicoEspera, comunEspera,
        empresaEspera, comunAtendido, empresarioAtendido, politicoAtendido, despertarEmpleadoComun, despertarEmpleadoEmpresa;

void atenderEmpresa(int n){
    printf("empleado %d esta atendiendo a una empresa\n",n);
}

void atenderPolitico(int n){
    printf("empleado %d esta atendiendo a un politico\n",n);
}

void atenderComun(int n){
    printf("empleado %d esta atendiendo a un cliente comun\n",n);
}


void *empleadoEmpresa(){
    while (1){
        recibirMensaje(1,despertarEmpleadoEmpresa);
        if (recibirMensajeSinEspera(1, politicoEspera) == -1){
            atenderPolitico(3);
            enviarMensaje(1, politicoAtendido);
        }
        else if (recibirMensajeSinEspera(1, empresaEspera) == -1){
            atenderEmpresa(3);//Esta atiendiendo
            enviarMensaje(1, empresarioAtendido);
        }
    }
}

void *empleadoComun(){
    while (1){
        recibirMensaje(1, despertarEmpleadoComun);
        if (recibirMensajeSinEspera(1, politicoEspera) == -1){
            atenderPolitico(3);
            enviarMensaje(1, politicoAtendido);}
        else if(recibirMensajeSinEspera(1, comunEspera) == -1){
            atenderComun(3);//Esta atiendiendo
            enviarMensaje(1, comunAtendido);
        }
    }
}

void *clienteComun(){
    if(recibirMensajeSinEspera(1, colaMesaEntrada)==-1){
        printf("Cliente Comun se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        recibirMensaje(1, colaClienteComun);
        enviarMensaje(1, colaMesaEntrada);
        enviarMensaje(1, comunEspera);
        enviarMensaje(1, despertarEmpleadoComun);
        recibirMensaje(1, comunAtendido);
        enviarMensaje(1, colaClienteComun);
    }else
        printf("La cola está llena, el cliente se va del banco\n");
}

void *clienteEmpresa(){

    if(recibirMensajeSinEspera(1, colaMesaEntrada) == -1){

        printf("Cliente EMPRESARIO se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        recibirMensaje(1, colaClienteEmpresa);
        enviarMensaje(1, colaMesaEntrada);
        enviarMensaje(1, empresaEspera);
        enviarMensaje(1, despertarEmpleadoEmpresa);
        enviarMensaje(1, despertarEmpleadoEmpresa);
        recibirMensaje(1, empresarioAtendido);
        enviarMensaje(1, colaClienteEmpresa);
    }else
        printf("La cola está llena, el cliente se va del banco\n");
}

void *clientePolitico(){
    if(recibirMensajeSinEspera(1, colaMesaEntrada) == -1){

        printf("Cliente Politico se unió a la colaMesaEntrada, aún hay espacios en la cola\n");
        recibirMensaje(1, colaClientePolitico);
        enviarMensaje(1, colaMesaEntrada);
        enviarMensaje(1, politicoEspera);
        enviarMensaje(1, despertarEmpleadoEmpresa);
        enviarMensaje(1, despertarEmpleadoEmpresa);
        enviarMensaje(1, despertarEmpleadoComun);
        recibirMensaje(1, politicoAtendido);
        enviarMensaje(1, colaClientePolitico);
    }else
        printf("La cola está llena, el cliente se va del banco\n");
}

int main() {

    colaClienteComun = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    colaClienteEmpresa = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    colaClientePolitico = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    colaMesaEntrada = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    politicoEspera = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    comunEspera = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    empresaEspera = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    comunAtendido = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    empresarioAtendido = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    politicoAtendido = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    despertarEmpleadoComun = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));
    despertarEmpleadoEmpresa = msgget(rand(), (IPC_CREAT | IPC_EXCL | 0600));

    for (int i = 0; i < 30; i++){
        enviarMensaje(1 , colaMesaEntrada);
    }
    for (int i = 0; i < 15; i++){
        enviarMensaje(1 , colaClienteComun);
        enviarMensaje(1 , colaClienteEmpresa);
        enviarMensaje(1 , colaClientePolitico);
    }

    for (int i=0; i<5 ; i++){
        if (!fork()){
            clienteComun();
        }
    }
    for (int i=0 ; i<5 ; i++){
        if (!fork()){
            clienteEmpresa();
        }
    }
    for (int i=0 ; i<5 ; i++){
        clientePolitico();
    }

    if (!fork()){
        empleadoComun();
    }
    for (int i=0 ; i<2 ; i++){
        if (!fork()){
            empleadoEmpresa();
        }
    }

    for (int i = 0; i < 5; i++){
        wait(NULL);
    }
    for (int i = 0; i < 5; i++){
        wait(NULL);
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

    exit(EXIT_SUCCESS);

}
