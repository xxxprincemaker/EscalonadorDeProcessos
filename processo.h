#include <stdlib.h>
#include<time.h>//necessário p/ função time()

#define QUANTUM 2
#define MAX_PROCESSOS 5
#define TEMPO_DISCO 2
#define TEMPO_FITA_MAGNETICA 3
#define TEMPO_IMPRESSORA 4

// Enumeração dos status para cada processo
typedef enum {HOLD, READY, RUNNING, WAITING, FINISHED} STATUS;
typedef enum {ALTA, BAIXA, IO} PRIORIDADE;

//Tipagem dos IO
typedef enum {DISCO, FITA_MAGNETICA, IMPRESSORA} TIPO_IO;
typedef struct INOUT {
    TIPO_IO tipo_io;
    int inicio;
} INOUT;

// Struct de cada processo
typedef struct Processo {
    long PID;
    PRIORIDADE prioridade;
    long PPID;
    STATUS status;
    unsigned inicio;
    unsigned servico_restante;
    INOUT io;
} Processo;


int processosAcabaram(Processo** processos, int n) {
    
    for(int i = 0; i < n; i++) {
        Processo *proc = processos[i];

        if(proc->servico_restante != 0) return 0;
    }
    return 1;
}

void criarProcessos(Processo **processos) {
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        Processo *proc = (Processo *) malloc(sizeof(Processo));

        proc->PID = i;
        proc->PPID = -1;
        proc->prioridade = ALTA;
        proc->status = HOLD;
        proc->inicio = i == 0 ? 0 : rand() % 10;
        proc->servico_restante = rand() % 5 + 1;

        processos[i] = proc;
    }
}

void tabelaDeProcessos(Processo **processos, int n) {
    printf("Processo\t| Inicio\t| T. de Servico\t|\n");
    printf("-------------------------------------------------\n");
    
    for(int i = 0; i < n; i++) {
        printf("Processo #%ld\t| %d\t\t| %d\t\t|\n", processos[i]->PID, processos[i]->inicio, processos[i]->servico_restante);
    }
    printf("\n\n");
}