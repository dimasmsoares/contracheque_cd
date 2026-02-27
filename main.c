#include <stdio.h>

#include "entrada.h"
#include "negocio.h"
#include "relatorio.h"

int main() {
    Servidor servidor = {0};

    printf("Simulador do Contracheque da Câmara dos Deputados\n");
    printf("Analista Legislativo\n\n");

    obter_dados_entrada(&servidor);
    calcular_rendimentos(&servidor);
    calcular_descontos(&servidor);

    limpar_terminal();
    gerar_relatorio(&servidor);

    return 0;
}
