#include  <unistd.h>
#include<stdio.h>
//#include<conio.h>
#include "fila.h"

void main() {

    Processo **processos = malloc(sizeof(Processo*) * MAX_PROCESSOS);

    Processo *proc_atual = (Processo *) NULL;   // Processo rodando atualmente na CPU

    Fila *fila_alta = criaFila(MAX_PROCESSOS);  // Fila de prioridade alta
    Fila *fila_baixa = criaFila(MAX_PROCESSOS); // Fila de prioridade baixa
    Fila *fila_io = criaFila(MAX_PROCESSOS);    // Fila de IO

    int t = 0; // Instante atual
    int t_quantum = 0; // Tempo passado no quantum atual

    srand(time(NULL));

    // Criar Processos, para depois Alocar na Fila.
    criarProcessos(processos);

    printf("Processo\t| Inicio\t| T. de Servico\t|\n");
    printf("-------------------------------------------------\n");
    
    for(int i = 0; i < MAX_PROCESSOS; i++) {
        printf("Processo #%ld\t| %d\t\t| %d\t\t|\n", processos[i]->PID, processos[i]->inicio, processos[i]->servico_restante);
    }
    printf("\n\n");

    printf("Fila de prioridade alta:\n");
    mostrarFila(fila_alta);
    printf("Fila de prioridade baixa:\n");
    mostrarFila(fila_baixa);
    printf("Fila de IO:\n");
    mostrarFila(fila_io);

    // TODO - Adicionar processos com IO


    // Para de rodar o loop quando todas as filas estiverem vazias
    // Não verifica as filas no instante t = 0, quando todas estão vazias
    while (!processosAcabaram(processos, MAX_PROCESSOS)) {
        
        // Percorre todos os processos procurando se existe um processo que inicia no instante t atual
        for(int i = 0; i < MAX_PROCESSOS; i++){

            Processo *proc = processos[i];

            // Se o tempo de ínicio for igual ao t atual,
            // coloca o processo na fila de prioridade alta
            if( proc->inicio == t ) {
                printf("[%04d] Processo #%ld iniciado (Entrou na fila de alta prioridade)\n", t, proc->PID);
                push(fila_alta, proc);
            }
        }

        // Se acabou o tempo do quantum atual
        if (proc_atual != (Processo *) NULL && t_quantum == QUANTUM ) {
            printf("[%04d] Processo #%ld saiu da CPU (Entrou na fila de baixa prioridade)\n", t, proc_atual->PID);
            push(fila_baixa, proc_atual);
            proc_atual = (Processo*) NULL;
            t_quantum = 0;
        }
        
        if( proc_atual == (Processo*) NULL ) {
            
            if( !isEmpty(fila_alta) ) {
                proc_atual = pop(fila_alta);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico_restante);
            } else if ( !isEmpty(fila_baixa) ) {
                proc_atual = pop(fila_baixa);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico_restante);
            }
        }

        t++;
        t_quantum++;
        if( proc_atual != (Processo*) NULL ) {
            proc_atual->servico_restante--;
            if( proc_atual->servico_restante == 0 ) { 
                printf("[%04d] Processo #%ld finalizou\n", t, proc_atual->PID);
                proc_atual = (Processo*) NULL;
                t_quantum = 0; 
            }
        } else {
            printf("[%04d] CPU oceosa\n", t);
        }
        sleep(1);
    }
    printf("Fim do Escalonador\n");
} 


