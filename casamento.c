#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "casamento.h"

char *lerArquivo(char *txt, int *tam) {

    FILE *arq = fopen(txt, "r");

    if (arq == NULL)   //caso ocorra algum erro
        return NULL;

    fseek(arq, 0, SEEK_END);
    int n = ftell(arq), i=0;
    fseek(arq, 0, SEEK_SET);

    char c;
    char *texto = (char*)malloc(sizeof(char)*(n));
    while (!feof(arq)) {
        c = fgetc(arq);
        if (c != EOF)
            texto[i] = c;
        i++;
    }
    *tam = n;

    fclose(arq);

    return texto;

}

//Boyer-Moore-Horspool
int bmh(char *padrao, char *texto, int n) {

    int i, j, k, m;
    int qtd = 0;

    //cria tabela de distancias das letras no padrao
    m = strlen(padrao);
    int *d = (int*)malloc(sizeof(int)*128);
    for (i = 0; i < 128; i++)
        d[i] = m;
    for (i = 0; i < m-1; i++)
        d[padrao[i]] = m-i-1;

    //faz a pesquisa
    i = m-1;
    while (i < n) {
        k = i;
        j = m-1;
        while (texto[k] == padrao[j] && j >= 0) {
            k--;
            j--;
        }
        if (j == -1) {
            printf("Casamento na posição: %d\n", k+1);
            qtd ++;
        }
        i += d[texto[i]];
    }

    free(d);

    return qtd;

}

//Boyer-Moore-Horspool-Sunday
int bmhs(char *padrao, char *texto, int n) {

    int i, j, k, m;
    int qtd = 0;

    //cria tabela de distancias das letras no padrao
    m = strlen(padrao);
    int *d = (int*)malloc(sizeof(int)*128);
    for (i = 0; i < 128; i++)
        d[i] = m+1;
    for (i = 0; i < m; i++)
        d[padrao[i]] = m-i;

    //faz a pesquisa
    i = m-1;
    while (i < n) {
        k = i;
        j = m-1;
        while (texto[k] == padrao[j] && j >= 0) {
            k--;
            j--;
        }
        if (j == -1) {
            printf("Casamento na posição: %d\n", k+1);
            qtd ++;
        }
        i += d[texto[i+1]];
    }

    free(d);

    return qtd;

}

//Shif-And
int shiftAnd(char *padrao, char *texto, int n) {

    int i, r = 0;
    int m = strlen(padrao);
    int *mascara = (int*)calloc(128, sizeof(int));
    int qtd = 0;
    for (i = 0; i < m; i++)
        mascara[padrao[i]] |= 1 << i;

    for (i = 0; i < n; i++) {
        r = ((r << 1) | 1) & mascara[texto[i]];
        if (r & (1 << (m-1))) {
            printf("Casamento na posição: %d\n", i-m+1);
            qtd ++;
        }
    }

    free(mascara);

    return qtd;

}

//Shif-And Aproximado
int shiftAndAprox(char *padrao, char *texto, int n) {

    int i, r = 0, r1 = 0, r_ant, r_temp;
    int m = strlen(padrao);
    int *mascara = (int*)calloc(128, sizeof(int));
    int qtd = 0;
    for (i = 0; i < m; i++)
        mascara[padrao[i]] |= 1 << i;

    for (i = 0; i < n; i++) {
        r_ant = r;
        r = ((r << 1) | 1) & mascara[texto[i]];
        r1 = ((((r1 << 1) | 1) & mascara[texto[i]]) | r_ant | (r_ant << 1));
        if (r & (1 << (m-1)) || r1 & (1 << (m-1))) {
            printf("Casamento na posição: %d\n", i-m+1);
            qtd ++;
        }
    }

    free(mascara);

    return qtd;

}

//Boyer-Moore-Horspool Paralelo
#define THREADS 4

typedef struct _parametro {
    char *texto;
    char *padrao;
    int *indices;
    int tam;
    int id;
    int *d;
} Parametro;

void *paralelo(void *parametro) {

    Parametro *p = (Parametro*)parametro;
    int i, j, k, m = p->tam, n;
    int *qtd = (int*)malloc(sizeof(int));
    *qtd = 0;

    i = m-1 + p->indices[p->id];
    while (i < p->indices[p->id + 1]) {
        k = i;
        j = m-1;
        while (p->texto[k] == p->padrao[j] && j >= 0) {
            k--;
            j--;
        }
        if (j == -1) {
            printf("Casamento na posição: %d\n", k+1);
            (*qtd) ++;
        }
        i += p->d[p->texto[i]];
    }

    return (void*)qtd;

}

int bmhParalelo(char *padrao, char *texto, int tam) {

    //cria tabela de distancias das letras no padrao

    // A criação da tabela é feita normalmente
    int m = strlen(padrao), i;
    int *d = (int*)malloc(sizeof(int)*128);
    for (i = 0; i < 128; i++)
        d[i] = m;
    for (i = 0; i < m-1; i++)
        d[padrao[i]] = m-i-1;

    int j = 1;
    int *indices = (int*)calloc(THREADS+1, sizeof(int));
    int qtd = 0, *qtd_retorno;

    for (i = tam/THREADS; i < tam; i++) {
        if(d[texto[i]] != m || texto[i] == padrao[m-1]) {
            indices[j] = i;
            j++;
            i += tam/THREADS - 1;
        }
    }

    indices[THREADS] = tam;
    Parametro *parametros = (Parametro*)malloc(sizeof(Parametro)*THREADS);
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t)*THREADS);
    for (i = 0; i < THREADS; i++) {
        parametros[i].indices = indices;
        parametros[i].id = i;
        parametros[i].texto = texto;
        parametros[i].padrao = padrao;
        parametros[i].d = d;
        parametros[i].tam = m;
        pthread_create(&(threads[i]), NULL, paralelo, &(parametros[i]));
    }

    for (i = 0; i < THREADS; i++) {
        pthread_join(threads[i], (void**)&qtd_retorno);
        qtd += *qtd_retorno;
        free(qtd_retorno);
    }

    free(d);
    free(indices);
    free(parametros);
    free(threads);

    return qtd;

}
