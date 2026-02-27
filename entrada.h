#ifndef ENTRADA_H
#define ENTRADA_H

#include "servidor.h"

void limpar_buffer();
void limpar_terminal();
int ler_inteiro(const char* mensagem, int min, int max);
char ler_confirmacao(const char* mensagem);
void obter_dados_entrada(Servidor *s);

#endif
