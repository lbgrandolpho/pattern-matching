#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "casamento.h"

enum Args {
    indiceEntrada, indicePadrao, indiceAlgoritmo
};

int main(int argc, char **argv) {

    int opt = 0, i, tam;
    int *args = (int*)malloc(sizeof(int)*3);
    char *texto;

    for (i=0; i < 3; i++)
        args[i] = -1;

    while (opt != -1) {
        opt = getopt(argc, argv, ":i:p:a:");  //pega as flags de parametro

        switch (opt) {
            case 'i': //arquivo de entrada
                args[indiceEntrada] = optind - 1;
                break;

            case 'p': //padrao que se deseja buscar
                args[indicePadrao] = optind - 1;
                break;

            case 'a': //algortimo de busca que será utilizado
                args[indiceAlgoritmo] = optind - 1;
                break;

            case '?': //ERRO
                printf("Opção %c inválida!\n", optopt);
        }
    }

    for (i=0; i < 3; i++)
        if (args[i] == -1) {
            free(args);
            return -1;
        }

    texto = lerArquivo(argv[args[indiceEntrada]], &tam);

    if (!strcmp(argv[args[indiceAlgoritmo]], "BMH"))
        bmh(argv[args[indicePadrao]], texto, tam);

    if (!strcmp(argv[args[indiceAlgoritmo]], "BMHS"))
        bmhs(argv[args[indicePadrao]], texto, tam);

    if (!strcmp(argv[args[indiceAlgoritmo]], "SHIFTAND"))
        shiftAnd(argv[args[indicePadrao]], texto, tam);

    if (!strcmp(argv[args[indiceAlgoritmo]], "SHIFTAND_APR"))
        shiftAndAprox(argv[args[indicePadrao]], texto, tam);

    if (!strcmp(argv[args[indiceAlgoritmo]], "BMH_PAR"))
        bmhParalelo(argv[args[indicePadrao]], texto, tam);

    free(texto);
    free(args);

}
