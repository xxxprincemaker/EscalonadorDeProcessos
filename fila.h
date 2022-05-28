//
// Created by VipSo on 5/26/2022.
//

#include <stdlib.h>
#include <limits.h>
#include "processo.h"

// Define a Estrutura.
typedef struct Fila {
    unsigned capacidade;
    int tamanho, frente, atras;
    Processo** array;
} Fila;

// Funcao para criar uma fila dada uma certa capacidade. Inicializa a fila com tamanho 0.
Fila* criaFila(unsigned capacidade) {
    Fila* fila = (Fila*)malloc(sizeof(Fila));

    fila->capacidade = capacidade;
    fila->frente = fila->tamanho = 0;
    fila->atras = capacidade - 1;
    
    fila->array = (Processo**)malloc(fila->capacidade * sizeof(Processo*));
    
    return fila;
}

// Função para saber se a Fila ta cheia.
int isFull(Fila* fila) {
    return (fila->tamanho == fila->capacidade);
}

// Checa se a Fila é vazia.
int isEmpty(Fila* fila) {
    return (fila->tamanho == 0);
}

// Função para adicionar objeto na Fila
void push(Fila* fila, Processo* item)
{
    if (isFull(fila)) return;

    fila->atras = (fila->atras + 1) % fila->capacidade;
    fila->array[fila->atras] = item;
    fila->tamanho = fila->tamanho + 1;
}

// Função para retirar um objeto da Fila
Processo* pop(Fila* fila)
{
    if (isEmpty(fila)) return NULL;

    Processo* item = fila->array[fila->frente];
    fila->frente = (fila->frente + 1) % fila->capacidade;
    fila->tamanho = fila->tamanho - 1;

    return item;
}

// Funcao para pegar o primeiro item da fila
Processo* front(Fila* fila)
{
    if (isEmpty(fila)) return NULL;

    return fila->array[fila->frente];
}

// Funcao para pegar o ultimo item da fila
Processo* rear(Fila* fila)
{
    if (isEmpty(fila)) return NULL;
    
    return fila->array[fila->atras];
}

void mostrarFila(struct Fila *fila){

	int cont, i;

    if (isEmpty(fila)) {
        printf("| Fila vazia |\n\n");
        return;
    }

    printf("| ");

    for ( i = 0; i < fila->tamanho; i++){

        printf("Processo #%ld | ", fila->array[i]->PID);

    }
	printf("\n\n");

}
