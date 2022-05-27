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

// Struct de cada processo
typedef struct Processo {
    long PID;
    PRIORIDADE prioridade;
    long PPID;
    STATUS status;
} Processo;

void atribuirPID(Processo processo)
{
    srand(time(NULL));
    processo.PID = rand() % 5;
}

void atribuirStatus(Processo processo, STATUS status)
{
    processo.status = status;
}

void atribuirPPID(Processo processo, long PPID)
{
    processo.PPID = PPID;
}

void atribuirPrioridade(Processo processo, PRIORIDADE prioridade)
{
    processo.prioridade = prioridade;
}