#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "mensaje.h"

struct msg_buffer {
    long msg_type;
    char msg_text[1];
} msg_entrada, msg_COLA_POLITICOS, msg_POLITICO_ATENDIDO, msg_ESPERANDO_POLITICO, msg_COLA_EMPRESARIOS, msg_EMPRESARIO_ATENDIDO, msg_ESPERANDO_EMPRESARIO,
        msg_COLA_CLIENTE_COMUN, msg_COMUN_ATENDIDO, msg_COMUN_ESPERANDO, msg_DESPERTAR_EMPLEADO_COMUN,
        msg_DESPERTAR_EMPLEADO_EMPRESA;

key_t key;
int id;

void atenderPolitico(int n){
    printf("empleado %d esta atendiendo a un politico\n",n);
    sleep(1);
}
void atenderEmpresa(int n){
    printf("empleado %d esta atendiendo a una empresa\n",n);
    sleep(1);
}
void atenderComun(int n){
    printf("empleado %d esta atendiendo a un cliente comun\n",n);
    sleep(1);
}



int clientePolitico(){
    if(msgrcv(id, &msg_entrada, MSGSIZE, ENTRADA, IPC_NOWAIT) != -1){ //Si hay espacio en la mesa de entrada el cliente ingresa, caso contrario se retira
        printf("politico ingresó en la colaMesasEntrada\n");

        msgrcv(id, &msg_COLA_POLITICOS, MSGSIZE, COLA_POLITICOS, 0); //espera a que haya lugar en la cola de politicos
        msgsnd(id, &msg_entrada, MSGSIZE, 0); //una vez ingresado a cola de politicos, libera el espacio en la mesa de entrada
        printf("politico ingresó en la colaPoliticos y libero espacio en colaMesaEntrada\n");

        msgsnd(id, &msg_ESPERANDO_POLITICO, MSGSIZE, 0); //politico avisa a los empleados que esta esperando
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_COMUN, MSGSIZE, 0); //despierta empleado comun
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_EMPRESA, MSGSIZE, 0); //despierta empleado empresa
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_EMPRESA, MSGSIZE, 0); //despierta empleado empresa

        msgrcv(id, &msg_POLITICO_ATENDIDO, MSGSIZE, POLITICO_ATENDIDO, 0); //espera a que el empleado lo atienda para irse del banco
        msgsnd(id, &msg_COLA_POLITICOS, MSGSIZE, 0); //Libero el espacio en la cola de politicos porque ya me voy.
        // politico se va del banco despues de ser atendido
    }
    else{
        printf("no hay lugar en la colaMesaEntrada, politico se retira\n");
        fflush(stdout);
    }
    return 0;
}

int clienteComun(){
    if(msgrcv(id, &msg_entrada, MSGSIZE, ENTRADA, IPC_NOWAIT) != -1){ //Si hay espacio en la mesa de entrada el cliente ingresa, caso contrario se retira
        printf("cliente comun ingresó en la colaMesasEntrada\n");

        msgrcv(id, &msg_COLA_CLIENTE_COMUN, MSGSIZE, COLA_CLIENTE_COMUN, 0); //espera a que haya lugar en la cola de clientes comunes
        msgsnd(id, &msg_entrada, MSGSIZE, 0); //ingreso a la cola de comunes, libera la mesa de entrada.
        printf("cliente comun ingresó en la colaClientesComunes y libero espacio en colaMesaEntrada\n");

        msgsnd(id, &msg_COMUN_ESPERANDO, MSGSIZE, 0); //cliente comun avisa al empleado capaz de atenderlo que esta esperando
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_COMUN, MSGSIZE, 0);

        msgrcv(id, &msg_COMUN_ATENDIDO, MSGSIZE, COMUN_ATENDIDO, 0); //espera a que el empleado lo atienda para irse del banco
        msgsnd(id, &msg_COLA_CLIENTE_COMUN, MSGSIZE, 0); //Libera la cola de clientescomunes
        // cliente comun se va del banco despues de ser atendido

    }
    else{
        printf("no hay lugar en la colaMesaEntrada, cliente comun se retira\n");
        fflush(stdout);
    }
    return 0;
}

int clienteEmpresa(){
    if(msgrcv(id, &msg_entrada, MSGSIZE, ENTRADA, IPC_NOWAIT) != -1){ //Si hay espacio en la mesa de entrada el cliente ingresa, caso contrario se retira
        printf("cliente empresario ingresó en la colaMesasEntrada\n");

        msgrcv(id, &msg_COLA_EMPRESARIOS, MSGSIZE, COLA_EMPRESARIOS, 0); //espera a que haya lugar en la cola de empresarios
        msgsnd(id, &msg_entrada, MSGSIZE, 0); //Paso a la cola de empresas, libera el espacio de la mesa de entrada.
        printf("cliente empresario ingresó en la colaEmpresarios y libero espacio en colaMesaEntrada\n");

        msgsnd(id, &msg_ESPERANDO_EMPRESARIO, MSGSIZE, 0); //cliente comun avisa a los empleado capaz de atenderlo que esta esperando
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_EMPRESA, MSGSIZE, 0);
        msgsnd(id, &msg_DESPERTAR_EMPLEADO_EMPRESA, MSGSIZE, 0);

        msgrcv(id, &msg_EMPRESARIO_ATENDIDO, MSGSIZE, EMPRESARIO_ATENDIDO, 0);
        msgsnd(id, &msg_COLA_EMPRESARIOS, MSGSIZE, 0); //Libera la cola de empresarios
        // cliente empresario se va del banco despues de ser atendido
    }
    else{
        printf("no hay lugar en la colaMesaEntrada, cliente comun se retira\n");
        fflush(stdout);
    }
    return 0;
}

int empleadoEmpresarios(int nro){
    while(1){
        msgrcv(id, &msg_DESPERTAR_EMPLEADO_EMPRESA, MSGSIZE, DESPERTAR_EMPLEADO_EMPRESA, 0); //espera a ser despertado por un cliente
        if (msgrcv(id, &msg_ESPERANDO_POLITICO, MSGSIZE, ESPERANDO_POLITICO, IPC_NOWAIT) != -1){  //si hay politicos esperando, atiende con prioridad
            atenderPolitico(nro);
            msgsnd(id, &msg_POLITICO_ATENDIDO, MSGSIZE, 0); //le avisa al politico que ya lo atendio
        }
        else if (msgrcv(id, &msg_ESPERANDO_EMPRESARIO, MSGSIZE, ESPERANDO_EMPRESARIO, IPC_NOWAIT) != -1){ //si hay empresarios esperando, atiende
            atenderEmpresa(nro);
            msgsnd(id, &msg_EMPRESARIO_ATENDIDO, MSGSIZE, 0); //le avisa al empresario que ya lo atendio
        }
    }
}

int empleadoComunes(int nro){
    while(1){
        msgrcv(id, &msg_DESPERTAR_EMPLEADO_COMUN, MSGSIZE, DESPERTAR_EMPLEADO_COMUN, 0); //espera a ser despertado por un cliente
        if (msgrcv(id, &msg_ESPERANDO_POLITICO, MSGSIZE, ESPERANDO_POLITICO, IPC_NOWAIT) != -1){ //si hay politicos esperando, atiende con prioridad
            atenderPolitico(nro);
            msgsnd(id, &msg_POLITICO_ATENDIDO, MSGSIZE, 0); //le avisa al politico que ya lo atendio
        }
        else if (msgrcv(id, &msg_COMUN_ESPERANDO, MSGSIZE, COMUN_ESPERANDO, IPC_NOWAIT) != -1){ //si hay clientes comunes esperando, atiende
            atenderComun(nro);
            msgsnd(id, &msg_COMUN_ATENDIDO, MSGSIZE, 0); //le avisa al cliente comun que ya lo atendio
        }
    }
}

int main(){
    int p, pid;

    strcpy(msg_entrada.msg_text, "E");
    strcpy(msg_COLA_POLITICOS.msg_text, "P");
    strcpy(msg_POLITICO_ATENDIDO.msg_text, "P");
    strcpy(msg_ESPERANDO_POLITICO.msg_text, "E");
    strcpy(msg_COLA_EMPRESARIOS.msg_text, "E");
    strcpy(msg_EMPRESARIO_ATENDIDO.msg_text, "E");
    strcpy(msg_ESPERANDO_EMPRESARIO.msg_text, "E");
    strcpy(msg_COLA_CLIENTE_COMUN.msg_text, "C");
    strcpy(msg_COMUN_ATENDIDO.msg_text, "C");
    strcpy(msg_COMUN_ESPERANDO.msg_text, "E");
    strcpy(msg_DESPERTAR_EMPLEADO_COMUN.msg_text, "D");
    strcpy(msg_DESPERTAR_EMPLEADO_EMPRESA.msg_text, "D");
    strcpy(msg_ESPERANDO_POLITICO.msg_text, "E");
    strcpy(msg_ESPERANDO_EMPRESARIO.msg_text, "E");
    strcpy(msg_COMUN_ESPERANDO.msg_text, "E");


    msg_entrada.msg_type = ENTRADA;
    msg_COLA_POLITICOS.msg_type = COLA_POLITICOS;
    msg_POLITICO_ATENDIDO.msg_type = POLITICO_ATENDIDO;
    msg_ESPERANDO_POLITICO.msg_type = ESPERANDO_POLITICO;
    msg_COLA_EMPRESARIOS.msg_type = COLA_EMPRESARIOS;
    msg_EMPRESARIO_ATENDIDO.msg_type = EMPRESARIO_ATENDIDO;
    msg_ESPERANDO_EMPRESARIO.msg_type = ESPERANDO_EMPRESARIO;
    msg_COLA_CLIENTE_COMUN.msg_type = COLA_CLIENTE_COMUN;
    msg_COMUN_ATENDIDO.msg_type = COMUN_ATENDIDO;
    msg_COMUN_ESPERANDO.msg_type = COMUN_ESPERANDO;
    msg_DESPERTAR_EMPLEADO_COMUN.msg_type = DESPERTAR_EMPLEADO_COMUN;
    msg_DESPERTAR_EMPLEADO_EMPRESA.msg_type = DESPERTAR_EMPLEADO_EMPRESA;
    msg_ESPERANDO_POLITICO.msg_type = ESPERANDO_POLITICO;
    msg_ESPERANDO_EMPRESARIO.msg_type = ESPERANDO_EMPRESARIO;
    msg_COMUN_ESPERANDO.msg_type = COMUN_ESPERANDO;


    key = ftok("/tmp", 'B');

    id = msgget(key, IPC_CREAT | 0666);

    msgctl(id, IPC_RMID, NULL);

    id = msgget(key, IPC_CREAT | 0666);

    if(id < 0){
        printf("Fallo el msgget. \n");
        exit(0);
    }

    for(p = 0; p < MAXIMO_MESA_ENTRADA; p++){
        msgsnd(id, &msg_entrada, MSGSIZE, 0);
    }

    for(p = 0; p < MAXIMO_COLAS_PARTICULARES; p++){
        msgsnd(id, &msg_COLA_POLITICOS, MSGSIZE, 0);
        msgsnd(id, &msg_COLA_EMPRESARIOS, MSGSIZE, 0);
        msgsnd(id, &msg_COLA_CLIENTE_COMUN, MSGSIZE, 0);
    }

    for(p = 0; p < CANTIDAD_POLITICOS; p++){
        pid = fork();
        if(pid == 0){
            clientePolitico(p);
            exit(0);
        }
    }

    if(pid != 0){
        for(p = 0; p < CANTIDAD_EMPRESAS; p++){
            pid = fork();
            if(pid == 0){
                clienteEmpresa(p);
                exit(0);
            }
        }
    }

    if(pid != 0){
        for(p = 0; p < CANTIDAD_COMUNES; p++){
            pid = fork();
            if(pid == 0){
                clienteComun(p);
                exit(0);
            }
        }
    }

    if(pid != 0){
        for(p = 0; p < 2; p++){
            pid = fork();
            if(pid == 0){
                empleadoEmpresarios(p);
                exit(0);
            }
        }
    }

    if(pid != 0){
        for(p = 0; p < 1; p++){
            pid = fork();
            if(pid == 0){
                empleadoComunes(2);
                exit(0);
            }
        }
    }

    if(pid != 0){
        for(p = 0; p < CANTIDAD_COMUNES + CANTIDAD_EMPRESAS + CANTIDAD_POLITICOS + 3; p++){
            wait(NULL);
        }
    }

    msgctl(id, IPC_RMID, NULL);

    return 0;
}