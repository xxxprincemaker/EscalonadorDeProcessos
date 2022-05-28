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
typedef enum {DISCO,FITA_MAGNETICA,IMPRESSORA} TIPO_IO;
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