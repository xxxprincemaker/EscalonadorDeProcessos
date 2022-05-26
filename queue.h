//
// Created by VipSo on 5/26/2022.
//

#include <stdlib.h>
#include <limits.h>

// Define a Estrutura.
struct Queue {
    int frente, atras, tamanho;
    unsigned capacidade;
    int* array;
};

// Funcao para criar uma Queue dada uma certa capacidade. Inicializa a queue com tamanho 0.
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(
            sizeof(struct Queue));
    queue->capacidade = capacity;
    queue->frente = queue->tamanho = 0;

    // This is important, see the enqueue
    queue->atras = capacity - 1;
    queue->array = (int*)malloc(
            queue->capacidade * sizeof(int));
    return queue;
}

// Função para saber se a Queue ta vazia.
int isFull(struct Queue* queue)
{
    return (queue->tamanho == queue->capacidade);
}

// Checa se a Queue é vazia.
int isEmpty(struct Queue* queue)
{
    return (queue->tamanho == 0);
}

// Função para adicionar objeto na Queue
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->atras = (queue->atras + 1)
                   % queue->capacidade;
    queue->array[queue->atras] = item;
    queue->tamanho = queue->tamanho + 1;
    printf("%d enqueued to queue\n", item);
}

// Função para Desenfilar um objeto da Queue
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->frente];
    queue->frente = (queue->frente + 1)
                    % queue->capacidade;
    queue->tamanho = queue->tamanho - 1;
    return item;
}

// Funcao para pegar o primeiro "objeto" da queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->frente];
}

// Funcao para pegar o ultimo "objeto" da queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->atras];
}

