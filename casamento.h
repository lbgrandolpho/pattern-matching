#ifndef CASAMENTO
#define CASAMENTO

char *lerArquivo(char *txt, int *tam);
int bmh(char *padrao, char *texto, int n);
int bmhs(char *padrao, char *texto, int n);
int shiftAnd(char *padrao, char *texto, int n);
int shiftAndAprox(char *padrao, char *texto, int n);
int bmhParalelo(char *padrao, char *texto, int tam);

#endif
