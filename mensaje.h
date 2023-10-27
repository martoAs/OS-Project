#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef struct msgStruct{
    long tipo;
    int count;
}tMsg;

int recibirMensajeSinEspera(int tMensaje, int msgid){
    tMsg mensaje;
    if (msgrcv(msgid, &mensaje, sizeof(tMsg)-sizeof(long), tMensaje, IPC_NOWAIT)==-1){
        if (errno != ENOMSG) {
            printf("Error al acceder a la cola de mensajes: %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }else{
            return -1;
        }
    }
    return 1;
}
tMsg recibirMensaje(int tMensaje, int msgid){
    tMsg mensaje;
    if (msgrcv(msgid, &mensaje, sizeof(mensaje)-sizeof(long), tMensaje, 0)==-1){
        printf("Error al acceder a la cola de mensajes: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    return mensaje;
}
void enviarMensaje(int tMensaje, int msgid){
    tMsg mensaje;
    mensaje.tipo = tMensaje;
    if (msgsnd(msgid, &mensaje, sizeof(mensaje)-sizeof(long), 0)==-1){
        printf("Error al acceder a la cola de mensajes: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
}
}
