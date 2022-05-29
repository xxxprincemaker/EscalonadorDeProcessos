#include<unistd.h>
#include<stdio.h>
#include "fila.h"

#define clear() printf("\033[H\033[J")
#define clearDown() printf("\033[0J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

void imprimeGraficoInicio(Processo** processos, int n);
void imprimeGrafico(Processo* proc_atual, int t, int n);
void imprimeTabelaEFilas(Processo** processos, int n, Processo* proc_atual, Fila* fila_alta, Fila* fila_baixa, Fila* fila_disco, Fila* fila_fita, Fila* fila_impressora);
void atualizaFilasIO(Fila* fila_disco, Fila* fila_fita, Fila* fila_impressora);
void imprimeFinal(Processo** processos, int n, int* t_finalizacoes);
void imprimeLog(FILE* log_file, int n_linhas, int x, int y);

void main() {

    Processo **processos = malloc(sizeof(Processo*) * MAX_PROCESSOS);

    Processo *proc_atual = (Processo *) NULL;   // Processo rodando atualmente na CPU

    Fila *fila_alta = criaFila(MAX_PROCESSOS);  // Fila de prioridade alta
    Fila *fila_baixa = criaFila(MAX_PROCESSOS); // Fila de prioridade baixa
    
    Fila *fila_disco = criaFila(MAX_PROCESSOS);    // Fila de IO de disco
    Fila *fila_fita = criaFila(MAX_PROCESSOS);    // Fila de IO de fita magnética
    Fila *fila_impressora = criaFila(MAX_PROCESSOS);    // Fila de IO de impressora

    FILE* log_file = fopen("log.txt", "w+");

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
        // Percorre todos os processos procurando se existe um processo que inicia no instante t atual
        for(int i = 0; i < MAX_PROCESSOS; i++){

            Processo *proc = processos[i];

            // Se o tempo de ínicio for igual ao t atual,
            // coloca o processo na fila de prioridade alta
            if( proc->inicio == t ) {
                if( proc_atual == (Processo*) NULL ) {
                    proc_atual = proc;
                    fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                    trocarStatus(proc, RUNNING);
                    t_quantum = 0;
                } else {
                    fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld iniciado (Entrou na fila de alta prioridade)\n", t, proc->PID);
                    trocarStatus(proc, READY);
                    trocarPrioridade(proc, ALTA);
                    push(fila_alta, proc);
                }
            }
        }

        if( proc_atual != (Processo *) NULL && proc_atual->tempo_passado == proc_atual->io.inicio && proc_atual->status != WAITING) {
            fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld iniciou IO (Entrou na fila de IO)\n", t, proc_atual->PID);
            trocarStatus(proc_atual, WAITING);
            if ( proc_atual->io.tipo_io == DISCO ) push(fila_disco, proc_atual);
            else if ( proc_atual->io.tipo_io == FITA_MAGNETICA ) push(fila_fita, proc_atual);
            else push(fila_impressora, proc_atual);
        }

        while( !isEmpty(fila_disco) && front(fila_disco)->io.tempo_restante == 0 ) {
            Processo* proc = pop(fila_disco);
            fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld finalizou IO de Disco (Entrou na fila de baixa prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            if( proc_atual == (Processo*) NULL || proc_atual->PID != proc->PID) {
                trocarPrioridade(proc, BAIXA);
                push(fila_baixa, proc);
            }
        }
        while( !isEmpty(fila_fita) && front(fila_fita)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_fita);
            fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld finalizou IO de Fita Magnetica (Entrou na fila de alta prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            if( proc_atual == (Processo*) NULL || proc_atual->PID != proc->PID) {
                trocarPrioridade(proc, ALTA);
                push(fila_alta, proc);
            } 
        }
        while( !isEmpty(fila_impressora) && front(fila_impressora)->io.tempo_restante == 0 )  {
            Processo* proc = pop(fila_impressora);
            fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld finalizou IO de Impressora (Entrou na fila de alta prioridade)\n", t, proc->PID);
            trocarStatus(proc, READY);
            if( proc_atual == (Processo*) NULL || proc_atual->PID != proc->PID) {
                trocarPrioridade(proc, ALTA);
                push(fila_alta, proc);
            }
        }

        // Se acabou o tempo do quantum atual
        if ( proc_atual != (Processo *) NULL && t_quantum == QUANTUM ) {
            if( proc_atual->status != WAITING ) {
                fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld saiu da CPU (Entrou na fila de baixa prioridade)\n", t, proc_atual->PID);
                trocarStatus(proc_atual, READY);
                trocarPrioridade(proc_atual, BAIXA);
                push(fila_baixa, proc_atual);
            }
            proc_atual = (Processo*) NULL;
            t_quantum = 0;
        }
        
        if( proc_atual == (Processo *) NULL && t_quantum == 0 ) {
            
            if( !isEmpty(fila_alta) ) {
                proc_atual = pop(fila_alta);
                fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                trocarStatus(proc_atual, RUNNING);
                trocarPrioridade(proc_atual, ALTA);
            } else if ( !isEmpty(fila_baixa) ) {
                proc_atual = pop(fila_baixa);
                fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld entrou na CPU [Tempo restante = %d]\n", t, proc_atual->PID, proc_atual->servico - proc_atual->tempo_passado);
                trocarStatus(proc_atual, RUNNING);
                trocarPrioridade(proc_atual, BAIXA);
            }
        }

        atualizaFilasIO(fila_disco, fila_fita, fila_impressora);

        imprimeGrafico(proc_atual, t, MAX_PROCESSOS);

        imprimeTabelaEFilas(processos, MAX_PROCESSOS, proc_atual, fila_alta, fila_baixa, fila_disco, fila_fita, fila_impressora);

        t++;
        if( proc_atual != (Processo*) NULL ) t_quantum++;
        if( proc_atual != (Processo*) NULL && proc_atual->status != WAITING ) {
            proc_atual->tempo_passado++;
            if( proc_atual->tempo_passado == proc_atual->servico ) {
                fprintf(log_file, "\033[34m[%04d]\033[0m Processo #%ld finalizou\n", t, proc_atual->PID);
                trocarStatus(proc_atual, FINISHED);
                t_finalizacoes[proc_atual->PID] = t-1;
                proc_atual = (Processo*) NULL;
                t_quantum = 0; 
            }
        } else {
            fprintf(log_file, "\033[34m[%04d]\033[0m CPU oceosa\n", t);
        }

        imprimeLog(log_file, 3*MAX_PROCESSOS + 13, 120, 0);

        sleep(1);
        gotoxy(1, 10);
        clearDown();
    }
    imprimeFinal(processos, MAX_PROCESSOS, t_finalizacoes);

    imprimeLog(log_file, 3*MAX_PROCESSOS + 13, 120, 0);

    fclose(log_file);
    gotoxy(1, 3*MAX_PROCESSOS + 15);
} 

void imprimeGraficoInicio(Processo** processos, int n) {
    printf("              \033[34mGráfico\033[0m\n");

    for(int i = 0; i < n; i++) {
        printf("\033[33mProcesso #%ld | \033[0m\n", processos[i]->PID);
    }
    printf("              0    5   10   15   20   25   30   35   40   45   50\n");
}

void imprimeGrafico(Processo* proc_atual, int t, int n) {
    if( proc_atual != (Processo *) NULL && proc_atual->status != WAITING ) {
        gotoxy(15 + t, (int) (2 + proc_atual->PID));
        printf("\033[34m█\033[0m");
    } else {
        for(int i = 0; i < n; i++) {
            gotoxy(15 + t, 2 + i);
            printf("\033[31m▒\033[0m");
        }
    }
}

void imprimeTabelaEFilas(Processo** processos, int n, Processo* proc_atual, Fila* fila_alta, Fila* fila_baixa, Fila* fila_disco, Fila* fila_fita, Fila* fila_impressora) {
    gotoxy(1, n + 5);
    tabelaDeProcessos(processos, n);

    printf("\033[34mFila de prioridade alta:\033[0m\n");
    mostrarFila(fila_alta);
    printf("\033[34mFila de prioridade baixa:\033[0m\n");
    mostrarFila(fila_baixa);
    printf("\033[34mFila de IO de Disco:\033[0m\n");
    mostrarFila(fila_disco);
    printf("\033[34mFila de IO de Fita:\033[0m\n");
    mostrarFila(fila_fita);
    printf("\033[34mFila de IO de Impressora:\033[0m\n");
    mostrarFila(fila_impressora);

    if (proc_atual != (Processo*) NULL) printf("\033[34mProcesso na CPU: \033[33mProcesso #%ld\033[0m\n\n", proc_atual->PID );
    else printf("\033[34mProcesso na CPU:\033[31m Nenhum\033[0m\n\n" );
}

void atualizaFilasIO(Fila* fila_disco, Fila* fila_fita, Fila* fila_impressora) {
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
}

void imprimeFinal(Processo** processos, int n, int* t_finalizacoes) {
    gotoxy(1, n + 5);
    tabelaDeProcessos(processos, n);

    printf("\033[31mFim do Escalonador\033[0m\n");

    printf("\n\n\033[34mTurnaround de cada processo:\033[33m\n");
    for(int i = 0; i < n; i++) {
        Processo* proc = processos[i];
        printf("\033[33mProcesso #%ld\033[0m - %ds\n", proc->PID, t_finalizacoes[i] - proc->inicio);
    }
}

void imprimeLog(FILE* log_file, int n_linhas, int x, int y) {
    int line_count = 0;
    
    fseek(log_file, 0, SEEK_SET);

    while (!feof(log_file)) {
        if(fgetc(log_file) == '\n') line_count++;
    }

    fseek(log_file, 0, SEEK_SET);

    char s[255];
    int count = 0, log_count = 0;
    while (fgets(s, 255, log_file) != NULL) {
        
        if(count >= line_count - n_linhas) {
            gotoxy(x, y + log_count + 1);
            printf("\033[K");
            printf("%s", s);
            log_count++;
        }
        count++;
    }
    
    fseek(log_file, 0, SEEK_END);
}