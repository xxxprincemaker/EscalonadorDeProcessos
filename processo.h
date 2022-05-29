#include <stdlib.h>
#include<time.h>//necessário p/ função time()

#define QUANTUM 2
#define MAX_PROCESSOS 5
#define TEMPO_DISCO 2
#define TEMPO_FITA_MAGNETICA 3
#define TEMPO_IMPRESSORA 4

// Enumeração dos status para cada processo
typedef enum {
    HOLD, READY, RUNNING, WAITING, FINISHED
} STATUS;
typedef enum {
    ALTA, BAIXA, IO
} PRIORIDADE;

//Tipagem dos IO
typedef enum {
    NAO_POSSUI, DISCO, FITA_MAGNETICA, IMPRESSORA
} TIPO_IO;
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


int processosAcabaram(Processo **processos, int n) {

    for (int i = 0; i < n; i++) {
        Processo *proc = processos[i];

        if (proc->servico != proc->tempo_passado) return 0;
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

        if (rand() % 2) {
            int r = rand();
            if (r % 3 == 0) {
                proc->io.tipo_io = DISCO;
                proc->io.tempo_restante = TEMPO_DISCO;
            } else if (r % 3 == 1) {
                proc->io.tipo_io = FITA_MAGNETICA;
                proc->io.tempo_restante = TEMPO_FITA_MAGNETICA;
            } else {
                proc->io.tipo_io = IMPRESSORA;
                proc->io.tempo_restante = TEMPO_IMPRESSORA;
            }

            proc->io.inicio = (rand() % (proc->servico - 1)) + 1;
        } else {
            proc->io.tipo_io = NAO_POSSUI;
            proc->io.inicio = -1;
        }

        processos[i] = proc;
    }
}

char *tipoIo(Processo *processo) {
    switch (processo->io.tipo_io) {

        case DISCO:
            return "Disco Rig.";
            break;
        case FITA_MAGNETICA:
            return "Fita Mag.";
            break;
        case IMPRESSORA:
            return "Impressora";
            break;
        default:
            return "Nao Possui";
            break;
    }
}

char *tipoStatus(Processo *processo) {
    switch (processo->status) {

        case HOLD:
            return "HOLD    ";
            break;
        case READY:
            return "READY    ";
            break;
        case RUNNING:
            return "RUNNING";
            break;
        case WAITING:
            return "WAITTING";
            break;
        case FINISHED:
            return "FINISHED";
            break;
    }
}

char *tipoPrioridade(Processo *processo) {
    switch (processo->prioridade) {
        case ALTA:
            return "ALTA  ";
            break;
        case BAIXA:
            return "BAIXA ";
            break;
        case IO:
            return "I/O";
            break;
    }
}

void trocarStatus(Processo *processo, STATUS status){
    processo->status = status;
}

void trocarPrioridade(Processo *processo, PRIORIDADE prioridade){
    processo->prioridade = prioridade;
}

void tabelaDeProcessos(Processo **processos, int n) {
    printf("\033[34mProcesso\t| Inicio\t| T. de Servico\t| Tipo de IO\t| Inicio do IO\t| STATUS \t| PRIRD.|\033[0m\n");
    printf("\033[34m---------------------------------------------------------------------------------------------------------\033[0m\n");
    char tempoinicio[5];
    char ppid[5];

    for (int i = 0; i < n; i++) {
        sprintf(tempoinicio, "%d", processos[i]->io.inicio);
        sprintf(ppid, "%ld", processos[i]->PPID);
        printf("\033[33mProcesso #%ld\t\033[34m|\033[0m %d\t\t\033[34m|\033[0m %d\t\t\033[34m|\033[0m %s\t\033[34m|\033[0m %s\t\t\033[34m|\033[0m %s\t\033[34m|\033[0m %s\033[34m|\033[0m\n", processos[i]->PID,
               processos[i]->inicio,
               processos[i]->servico,
               tipoIo(processos[i]), (processos[i]->io.inicio == -1 ? "-" : tempoinicio), tipoStatus(processos[i]),
               tipoPrioridade(processos[i]));

    }
    printf("\n\n");
}