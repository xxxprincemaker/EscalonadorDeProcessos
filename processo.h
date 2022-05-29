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
    unsigned tempo_restante;
} INOUT;

// Struct de cada processo
typedef struct Processo {
    long PID;
    PRIORIDADE prioridade;
    long PPID;
    STATUS status;
    unsigned inicio;
    unsigned servico;
    unsigned tempo_passado;
    INOUT io;
} Processo;


int processosAcabaram(Processo** processos, int n) {
    
    for(int i = 0; i < n; i++) {
        Processo *proc = processos[i];

        if(proc->servico != proc->tempo_passado) return 0;
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
        proc->servico = rand() % 5 + 3;
        proc->tempo_passado = 0;

        if(rand() % 2) {
            int r = rand();
            if(r % 3 == 0) {
                proc->io.tipo_io = DISCO;
                proc->io.tempo_restante = TEMPO_DISCO;
            }
            else if (r % 3 == 1) {
                proc->io.tipo_io = FITA_MAGNETICA;
                proc->io.tempo_restante = TEMPO_FITA_MAGNETICA;
            }
            else {
                proc->io.tipo_io = IMPRESSORA;
                proc->io.tempo_restante = TEMPO_IMPRESSORA;
            }

            proc->io.inicio = (rand() % (proc->servico-1)) + 1;
        } else {
            proc->io.inicio = -1;
        }

        processos[i] = proc;
    }
}

void tabelaDeProcessos(Processo **processos, int n) {
    printf("Processo\t| Inicio\t| T. de Servico\t| Tipo de IO\t| Início do IO\t|\n");
    printf("---------------------------------------------------------------------------------\n");
    
    for(int i = 0; i < n; i++) {
        if ( processos[i]->io.tipo_io == DISCO ) 
            printf("Processo #%ld\t| %d\t\t| %d\t\t| %s\t\t| %d\t\t|\n", processos[i]->PID, processos[i]->inicio, processos[i]->servico, "Disco", processos[i]->io.inicio);
        else if ( processos[i]->io.tipo_io == FITA_MAGNETICA ) 
            printf("Processo #%ld\t| %d\t\t| %d\t\t| %s| %d\t\t|\n", processos[i]->PID, processos[i]->inicio, processos[i]->servico, "Fita Magnética", processos[i]->io.inicio);
        else 
            printf("Processo #%ld\t| %d\t\t| %d\t\t| %s\t| %d\t\t|\n", processos[i]->PID, processos[i]->inicio, processos[i]->servico, "Impressora", processos[i]->io.inicio);
    }
    printf("\n\n");
}