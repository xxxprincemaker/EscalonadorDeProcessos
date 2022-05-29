#include  <unistd.h>
#include<stdio.h>
//#include<conio.h>
#include "fila.h"

#define clear() printf("\033[H\033[J")
#define clearDown() printf("\033[0J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

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

    clear();
    srand(time(NULL));

    // Criar Processos, para depois Alocar na Fila.
    criarProcessos(processos);

    imprimeGraficoInicio(processos, MAX_PROCESSOS);

    // Para de rodar o loop quando todas as filas estiverem vazias
    // Não verifica as filas no instante t = 0, quando todas estão vazias
    while (!processosAcabaram(processos, MAX_PROCESSOS)) {
        gotoxy(1, 10);
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

        int l = 10;
        int x = 120;
        // Percorre todos os processos procurando se existe um processo que inicia no instante t atual
        for(int i = 0; i < MAX_PROCESSOS; i++){

            Processo *proc = processos[i];

            // Se o tempo de ínicio for igual ao t atual,
            // coloca o processo na fila de prioridade alta
            if( proc->inicio == t ) {
                if( proc_atual == (Processo*) NULL ) {
                    proc_atual = proc;
                    gotoxy(x, l++);
                    printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                    trocarStatus(proc, RUNNING);
                    t_quantum = 0;
                } else {
                    gotoxy(x, l++);
                    printf("[%04d] Processo #%ld iniciado (Entrou na fila de alta prioridade)\n", t, proc->PID);
                    trocarStatus(proc, READY);
                    trocarPrioridade(proc, ALTA);
                    push(fila_alta, proc);
                }
            }
        }

        while( !isEmpty(fila_disco) && front(fila_disco)->io.tempo_restante == 0 ) {
            Processo* proc = pop(fila_disco);
            gotoxy(x, l++);
            printf("[%04d] Processo #%ld finalizou IO de Disco (Entrou na fila de baixa prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            trocarPrioridade(proc, BAIXA);
            push(fila_baixa, proc);
        }
        while( !isEmpty(fila_fita) && front(fila_fita)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_fita);
            gotoxy(x, l++);
            printf("[%04d] Processo #%ld finalizou IO de Fita Magnetica (Entrou na fila de alta prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            trocarPrioridade(proc, ALTA);
            push(fila_alta, proc);
        }
        while( !isEmpty(fila_impressora) && front(fila_impressora)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_impressora);
            gotoxy(x, l++);
            printf("[%04d] Processo #%ld finalizou IO de Impressora (Entrou na fila de alta prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            trocarPrioridade(proc, ALTA);
            push(fila_alta, proc);
        }

        if( proc_atual != (Processo *) NULL && proc_atual->tempo_passado == proc_atual->io.inicio ) {
            gotoxy(x, l++);
            printf("[%04d] Processo #%ld iniciou IO (Entrou na fila de IO)\n", t, proc_atual->PID);
            trocarStatus(proc_atual, WAITING);
            if ( proc_atual->io.tipo_io == DISCO ) push(fila_disco, proc_atual);
            else if ( proc_atual->io.tipo_io == FITA_MAGNETICA ) push(fila_fita, proc_atual);
            else push(fila_impressora, proc_atual);
            
            proc_atual = (Processo*) NULL;
        }

        // Se acabou o tempo do quantum atual
        if ( t_quantum == QUANTUM ) {
            if ( proc_atual != (Processo *) NULL ) {
                gotoxy(x, l++);
                printf("[%04d] Processo #%ld saiu da CPU (Entrou na fila de baixa prioridade)\n", t, proc_atual->PID);
                trocarStatus(proc_atual, READY);
                trocarPrioridade(proc_atual, BAIXA);
                push(fila_baixa, proc_atual);
                proc_atual = (Processo*) NULL;
            }
            t_quantum = 0;
        }
        
        if( proc_atual == (Processo *) NULL && t_quantum == 0 ) {
            
            if( !isEmpty(fila_alta) ) {
                proc_atual = pop(fila_alta);
                gotoxy(x, l++);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                trocarStatus(proc_atual, RUNNING);
                trocarPrioridade(proc_atual, ALTA);
            } else if ( !isEmpty(fila_baixa) ) {
                proc_atual = pop(fila_baixa);
                gotoxy(x, l++);
                printf("[%04d] Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                trocarStatus(proc_atual, RUNNING);
                trocarPrioridade(proc_atual, BAIXA);
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

        imprimeGrafico(proc_atual, t, MAX_PROCESSOS);

        t++;
        t_quantum++;
        if( proc_atual != (Processo*) NULL ) {
            proc_atual->tempo_passado++;
            if( proc_atual->tempo_passado == proc_atual->servico ) { 
                gotoxy(x, l++);
                printf("[%04d] Processo #%ld finalizou\n", t, proc_atual->PID);
                trocarStatus(proc_atual, FINISHED);
                t_finalizacoes[proc_atual->PID] = t-1;
                proc_atual = (Processo*) NULL;
                t_quantum = 0; 
            }
        } else {
            gotoxy(x, l++);
            printf("[%04d] CPU oceosa\n", t);
        }
        sleep(1);
        gotoxy(1, 10);
        clearDown();
    }
    gotoxy(1, 10);
    tabelaDeProcessos(processos, MAX_PROCESSOS);

    printf("Fim do Escalonador\n");

    printf("\n\nTurnaround de cada processo:\n");
    for(int i = 0; i < MAX_PROCESSOS; i++) {
        Processo* proc = processos[i];
        printf("Processo #%ld - %ds\n", proc->PID, t_finalizacoes[i] - proc->inicio);
    }
} 

void imprimeGraficoInicio(Processo** processos, int n) {
    printf("              Gráfico\n");

    for(int i = 0; i < n; i++) {
        printf("\033[33mProcesso #%ld | \n\033[0m", processos[i]->PID);
    }
    printf("              0    5   10   15   20   25   30   35   40   45   50\n");
}

void imprimeGrafico(Processo* proc_atual, int t, int n) {
    if(proc_atual != (Processo *) NULL) {
        gotoxy(15 + t, 2 + proc_atual->PID);
        printf("\033[34m█\033[0m");
    } else {
        for(int i = 0; i < n; i++) {
            gotoxy(15 + t, 2 + i);
            printf("\033[31m▒\033[0m");
        }
    }
}