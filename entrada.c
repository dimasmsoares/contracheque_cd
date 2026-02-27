#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "constantes.h"
#include "entrada.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_terminal() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int ler_inteiro(const char* mensagem, int min, int max) {
    int valor;
    int entrada_valida = 0;

    do {
        printf("%s", mensagem);
        if (scanf("%d", &valor) == 1) {
            if (valor >= min && valor <= max) {
                entrada_valida = 1;
            } else {
                printf("Valor inválido! Digite entre %d e %d.\n", min, max);
            }
        } else {
            printf("Entrada inválida! Digite apenas números.\n");
        }
        limpar_buffer();
    } while (!entrada_valida);

    return valor;
}

char ler_confirmacao(const char* mensagem) {
    char opcao;
    do {
        printf("%s (s/n): ", mensagem);
        opcao = getchar();
        limpar_buffer();

        opcao = toupper(opcao);
        if (opcao == 'S' || opcao == 'N') {
            return opcao;
        }
        printf("Opção inválida. Digite 's' para SIM ou 'n' para NÃO.\n");
    } while (1);
}

void obter_dados_entrada(Servidor *s) {
    s->padrao = ler_inteiro("Digite o seu padrão da carreira (1 a 10): ", 1, 10);

    float perc_doutorado = (ler_confirmacao("Possui doutorado?") == 'S') ? 10.0f : 0.0f;
    float perc_mestrado = (ler_confirmacao("Possui mestrado?") == 'S') ? 8.0f : 0.0f;

    printf("\nPossui especializações?\n1 - Não\n2 - 1ª Esp.\n3 - 1ª e 2ª Esp.\n");
    int op_esp = ler_inteiro("Sua opção: ", 1, 3);
    float perc_pos = (op_esp == 2) ? 6.0f : (op_esp == 3 ? 9.0f : 0.0f);

    float perc_grad = (ler_confirmacao("Possui 2ª graduação?") == 'S') ? 21.0f : 15.0f;

    float total_perc = perc_doutorado + perc_mestrado + perc_pos + perc_grad;
    if (total_perc > 30.0f) total_perc = 30.0f;
    s->ad_especializacao = VENC_BASICO[9] * (total_perc / 100.0f);

    s->ad_ferias = (ler_confirmacao("Adicional de Férias?") == 'S') ? -1.0f : 0.0f;

    printf("\nFunção comissionada (0 p/ não possuir, 1-6 para FC):\n");
    int op_fc = ler_inteiro("Sua opção: ", 0, 6);
    s->funcao_comissionada = (op_fc == 0) ? 0.0f : TAB_FC[op_fc - 1];

    printf("\nRegime Previdenciário:\n1 - RPPS\n2 - RGPS + FUNPRESP\n3 - RGPS\n");
    s->regime_previdenciario = ler_inteiro("Sua opção: ", 1, 3);

    if (s->regime_previdenciario == 2) {
        printf("\nPercentual FUNPRESP:\n1 - 7.5%%\n2 - 8.0%%\n3 - 8.5%%\n");
        int op_funp = ler_inteiro("Sua opção: ", 1, 3);
        s->aliquota_funpresp = (op_funp == 1) ? 0.075f : (op_funp == 2 ? 0.080f : 0.085f);
    } else {
        s->aliquota_funpresp = 0.0f;
    }

    if (ler_confirmacao("Aderiu ao Pró-Saúde?") == 'S') {
        int idade = ler_inteiro("Digite sua idade: ", 0, 150);
        s->pro_saude = TAB_PRO_SAUDE[1][9];
        for (int i = 0; i < 10; i++) {
            if (idade <= TAB_PRO_SAUDE[0][i]) {
                s->pro_saude = TAB_PRO_SAUDE[1][i];
                break;
            }
        }
    } else {
        s->pro_saude = 0.0f;
    }

    if (ler_confirmacao("Sessões Noturnas?") == 'S') {
        int qnt_sessoes = ler_inteiro("Quantas sessões (2h): ", 1, 30);
        s->sessao_noturna = qnt_sessoes * 485.75f;
    } else {
        s->sessao_noturna = 0.0f;
    }
}
