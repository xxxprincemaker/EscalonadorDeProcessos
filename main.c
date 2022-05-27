#include<stdio.h>
//#include<conio.h>
#include "fila.h"

void main() {
    Fila *fila = criaFila(MAX_PROCESSOS);
    //Criar Processos, para depois Alocar na Fila.
    int i=0;
    for(i=0; i<MAX_PROCESSOS; i++) {
        Processo* proc = (Processo*)malloc(sizeof(Processo));

        proc->PID = i;
        proc->PPID = -1;
        proc->prioridade = ALTA;
        proc->status = HOLD;

        push(fila, proc);
    }

    printf("Fila:\n");
    mostrarFila(fila);

    //while(!isEmpty(fila)){
    //    
    //}
} 