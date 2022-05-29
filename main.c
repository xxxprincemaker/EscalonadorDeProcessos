#include  <unistd.h>
#include<stdio.h>
//#include<conio.h>
#include "fila.h"

void main() {

    Processo **processos = malloc(sizeof(Processo*) * MAX_PROCESSOS);

    Processo *proc_atual = (Processo *) NULL;   // Processo rodando atualmente na CPU

    Fila *fila_alta = criaFila(MAX_PROCESSOS);  // Fila de prioridade alta
    Fila *fila_baixa = criaFila(MAX_PROCESSOS); // Fila de prioridade baixa
    
    Fila *fila_disco = criaFila(MAX_PROCESSOS);    // Fila de IO de disco
    Fila *fila_fita = criaFila(MAX_PROCESSOS);    // Fila de IO de fita magnética
    Fila *fila_impressora = criaFila(MAX_PROCESSOS);    // Fila de IO de impressora

    // Tempos de finalizações de cada processo
    // Usados para calcular o turnaround de cada processo
    int* t_finalizacoes = malloc(sizeof(int) * MAX_PROCESSOS);

    int t = 0; // Instante atual
    int t_quantum = 0; // Tempo passado no quantum atual

    srand(time(NULL));

    // Criar Processos, para depois Alocar na Fila.
    criarProcessos(processos);

    
    

    // TODO - Adicionar processos com IO


    // Para de rodar o loop quando todas as filas estiverem vazias
    // Não verifica as filas no instante t = 0, quando todas estão vazias
    while (!processosAcabaram(processos, MAX_PROCESSOS)) {
        tabelaDeProcessos(processos, MAX_PROCESSOS);

        printf("Fila de prioridade alta:\n");
        mostrarFila(fila_alta);
        printf("Fila de prioridade baixa:\n");
        mostrarFila(fila_baixa);
        printf("Fila de IO de Disco:\n");
        mostrarFila(fila_disco);
        printf("Fila de IO de Fita:\n");
        mostrarFila(fila_fita);
        printf("Fila de IO de Impressora:\n");
        mostrarFila(fila_impressora);

        printf("Processo na CPU: Processo #%ld\n\n", (proc_atual != (Processo*) NULL ? proc_atual->PID : -1) );
        // Percorre todos os processos procurando se existe um processo que inicia no instante t atual
        for(int i = 0; i < MAX_PROCESSOS; i++){

            Processo *proc = processos[i];

            // Se o tempo de ínicio for igual ao t atual,
            // coloca o processo na fila de prioridade alta
            if( proc->inicio == t ) {
                if( proc_atual == (Processo*) NULL ) {
                    proc_atual = proc;
                    printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                    t_quantum = 0;
                } else {
                    printf("[%04d] Processo #%ld iniciado (Entrou na fila de alta prioridade)\n", t, proc->PID);
                    push(fila_alta, proc);
                }
            }
        }

        while( !isEmpty(fila_disco) && front(fila_disco)->io.tempo_restante == 0 ) {
            Processo* proc = pop(fila_disco);
            printf("[%04d] Processo #%ld finalizou IO de Disco (Entrou na fila de baixa prioridade)\n", t, proc->PID);
            push(fila_baixa, proc);
        }
        while( !isEmpty(fila_fita) && front(fila_fita)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_fita);
            printf("[%04d] Processo #%ld finalizou IO de Fita Magnetica (Entrou na fila de alta prioridade)\n", t, proc->PID);
            push(fila_alta, proc);
        }
        while( !isEmpty(fila_impressora) && front(fila_impressora)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_impressora);
            printf("[%04d] Processo #%ld finalizou IO de Impressora (Entrou na fila de alta prioridade)\n", t, proc->PID);
            push(fila_alta, proc);
        }

        if( proc_atual != (Processo *) NULL && proc_atual->tempo_passado == proc_atual->io.inicio ) {
            printf("[%04d] Processo #%ld iniciou IO (Entrou na fila de IO)\n", t, proc_atual->PID);
            if ( proc_atual->io.tipo_io == DISCO ) push(fila_disco, proc_atual);
            else if ( proc_atual->io.tipo_io == FITA_MAGNETICA ) push(fila_fita, proc_atual);
            else push(fila_impressora, proc_atual);
            
            proc_atual = (Processo*) NULL;
        }

        // Se acabou o tempo do quantum atual
        if ( t_quantum == QUANTUM ) {
            if ( proc_atual != (Processo *) NULL ) {
                printf("[%04d] Processo #%ld saiu da CPU (Entrou na fila de baixa prioridade)\n", t, proc_atual->PID);
                push(fila_baixa, proc_atual);
                proc_atual = (Processo*) NULL;
            }
            t_quantum = 0;
        }
        
        if( proc_atual == (Processo *) NULL && t_quantum == 0 ) {
            
            if( !isEmpty(fila_alta) ) {
                proc_atual = pop(fila_alta);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
            } else if ( !isEmpty(fila_baixa) ) {
                proc_atual = pop(fila_baixa);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
            }
        }

        for( int i = 0; i < fila_disco->tamanho; i++ ) {
            int fila_pos = (fila_disco->frente + i) % fila_disco->capacidade;
            fila_disco->array[fila_pos]->io.tempo_restante--;
        }

        for( int i = 0; i < fila_fita->tamanho; i++ ) {
            int fila_pos = (fila_fita->frente + i) % fila_fita->capacidade;
            fila_fita->array[fila_pos]->io.tempo_restante--;
        }

        for( int i = 0; i < fila_impressora->tamanho; i++ ) {
            int fila_pos = (fila_impressora->frente + i) % fila_impressora->capacidade;
            fila_impressora->array[fila_pos]->io.tempo_restante--;
        }

        t++;
        t_quantum++;
        if( proc_atual != (Processo*) NULL ) {
            proc_atual->tempo_passado++;
            if( proc_atual->tempo_passado == proc_atual->servico ) { 
                printf("[%04d] Processo #%ld finalizou\n", t, proc_atual->PID);
                t_finalizacoes[proc_atual->PID] = t;
                proc_atual = (Processo*) NULL;
                t_quantum = 0; 
            }
        } else {
            printf("[%04d] CPU oceosa\n", t);
        }
        sleep(1);
        printf("\033[2J");
    }
    printf("Fim do Escalonador\n");

    printf("Turnaround de cada processo:\n");
    for(int i = 0; i < MAX_PROCESSOS; i++) {
        Processo* proc = processos[i];
        printf("Processo #%ld - %ds\n", proc->PID, t_finalizacoes[i] - proc->inicio);
    }
} 