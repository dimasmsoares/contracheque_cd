/******************************Headers*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // Para a função toupper()
#include "constantes.h"

/******************************Estruturas**************************************/
typedef struct {
    int padrao;
    int regime_previdenciario;
    float aliquota_funpresp;

    float rem_bruta;
    float rem_liquida;

    // Adicionais
    float ad_especializacao;
    float ad_ferias;
    float aux_pre_escolar;
    float funcao_comissionada;
    float sessao_noturna;

    // Descontos
    float cont_funpresp;
    float cont_rgps;
    float cont_rpps;
    float pro_saude;
    float imposto_renda;
} Servidor;

/******************************Protótipos**************************************/
// Funções Utilitárias
void limpar_buffer();
void limpar_terminal();
int ler_inteiro(const char* mensagem, int min, int max);
char ler_confirmacao(const char* mensagem);

// Lógica de Negócio
void obter_dados_entrada(Servidor *s);
float calcular_progressivo(float base_calculo, const float limites[], const float aliquotas[], int faixas);
float calcular_ir_progressivo(float base_calculo);

void calcular_rendimentos(Servidor *s);
void calcular_descontos(Servidor *s);
void gerar_relatorio(Servidor *s);

/******************************Main********************************************/
int main() {
    Servidor servidor = {0}; // Inicializa tudo com zero

    printf("Simulador do Contracheque da Câmara dos Deputados\n");
    printf("Analista Legislativo\n\n");

    obter_dados_entrada(&servidor);
    calcular_rendimentos(&servidor);
    calcular_descontos(&servidor);

    limpar_terminal();
    gerar_relatorio(&servidor);

    return 0;
}

/******************************Implementação***********************************/

/* --- Utilitários --- */

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

// Lê um número inteiro validando o intervalo
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

// Lê 's' ou 'n'
char ler_confirmacao(const char* mensagem) {
    char opcao;
    do {
        printf("%s (s/n): ", mensagem);
        opcao = getchar();
        limpar_buffer(); // Limpa o enter

        opcao = toupper(opcao); // Converte para maiúscula
        if (opcao == 'S' || opcao == 'N') {
            return opcao;
        }
        printf("Opção inválida. Digite 's' para SIM ou 'n' para NÃO.\n");
    } while (1);
}

/* --- Lógica de Negócio: Entradas --- */

void obter_dados_entrada(Servidor *s) {
    // 1. Padrão
    s->padrao = ler_inteiro("Digite o seu padrão da carreira (1 a 10): ", 1, 10);

    // 2. Especialização
    float perc_doutorado = (ler_confirmacao("Possui doutorado?") == 'S') ? 10.0f : 0.0f;
    float perc_mestrado = (ler_confirmacao("Possui mestrado?") == 'S') ? 8.0f : 0.0f;

    printf("\nPossui especializações?\n1 - Não\n2 - 1ª Esp.\n3 - 1ª e 2ª Esp.\n");
    int op_esp = ler_inteiro("Sua opção: ", 1, 3);
    float perc_pos = (op_esp == 2) ? 6.0f : (op_esp == 3 ? 9.0f : 0.0f);

    float perc_grad = (ler_confirmacao("Possui 2ª graduação?") == 'S') ? 21.0f : 15.0f;

    float total_perc = perc_doutorado + perc_mestrado + perc_pos + perc_grad;
    if (total_perc > 30.0f) total_perc = 30.0f;

    // Calcula valor absoluto baseado no topo da carreira (índice 9)
    s->ad_especializacao = VENC_BASICO[9] * (total_perc / 100.0f);

    // 3. Adicional de Férias
    s->ad_ferias = (ler_confirmacao("Adicional de Férias?") == 'S') ? -1.0f : 0.0f;

    // 4. Função Comissionada
    printf("\nFunção comissionada (0 p/ não possuir, 1-6 para FC):\n");
    int op_fc = ler_inteiro("Sua opção: ", 0, 6);
    s->funcao_comissionada = (op_fc == 0) ? 0.0f : TAB_FC[op_fc - 1];

    // 5. Previdência
    printf("\nRegime Previdenciário:\n1 - RPPS\n2 - RGPS + FUNPRESP\n3 - RGPS\n");
    s->regime_previdenciario = ler_inteiro("Sua opção: ", 1, 3);

    if (s->regime_previdenciario == 2) {
        printf("\nPercentual FUNPRESP:\n1 - 7.5%%\n2 - 8.0%%\n3 - 8.5%%\n");
        int op_funp = ler_inteiro("Sua opção: ", 1, 3);
        s->aliquota_funpresp = (op_funp == 1) ? 0.075f : (op_funp == 2 ? 0.080f : 0.085f);
    } else {
        s->aliquota_funpresp = 0.0f;
    }

    // 6. Pró-Saúde
    if (ler_confirmacao("Aderiu ao Pró-Saúde?") == 'S') {
        int idade = ler_inteiro("Digite sua idade: ", 0, 150);
        s->pro_saude = TAB_PRO_SAUDE[1][9]; // Default para maior idade
        for (int i = 0; i < 10; i++) {
            if (idade <= TAB_PRO_SAUDE[0][i]) {
                s->pro_saude = TAB_PRO_SAUDE[1][i];
                break;
            }
        }
    } else {
        s->pro_saude = 0.0f;
    }

    // 7. Sessões Noturnas
    if (ler_confirmacao("Sessões Noturnas?") == 'S') {
        int qnt_sessoes = ler_inteiro("Quantas sessões (2h): ", 1, 30);
        s->sessao_noturna = qnt_sessoes * 485.75f;
    } else {
        s->sessao_noturna = 0.0f;
    }
}

/* --- Lógica de Negócio: Cálculos --- */

float calcular_progressivo(float base, const float limites[], const float aliquotas[], int faixas) {
    float imposto = 0.0f;

    if (base <= 0.0f) {
        return 0.0f;
    }

    for (int i = 0; i < faixas; i++) {
        float limite_inferior = (i == 0) ? 0.0f : limites[i - 1];
        float limite_superior = limites[i];

        if (base > limite_superior) {
            imposto += (limite_superior - limite_inferior) * aliquotas[i];
        } else {
            imposto += (base - limite_inferior) * aliquotas[i];
            break;
        }
    }
    return imposto;
}

float calcular_ir_progressivo(float base) {
    return calcular_progressivo(base, TABELA_IR[0], TABELA_IR[1], 5);
}

void calcular_rendimentos(Servidor *s) {
    float soma_base = VENC_BASICO[s->padrao - 1] +
                      GRAT_REPRESENTACAO +
                      s->ad_especializacao +
                      GRAT_ATV_LEG[s->padrao - 1] +
                      VPI +
                      AUX_ALIMENTACAO +
                      s->funcao_comissionada;

    if (s->ad_ferias < 0.0f) {
        s->ad_ferias = (soma_base - AUX_ALIMENTACAO) / 3.0f;
    }

    s->rem_bruta = soma_base + s->ad_ferias + s->sessao_noturna;
}

void calcular_descontos(Servidor *s) {
    s->cont_rpps = 0.0f;
    s->cont_rgps = 0.0f;
    s->cont_funpresp = 0.0f;

    if (s->regime_previdenciario == 1) {
        s->cont_rpps = calcular_progressivo(s->rem_bruta, TABELA_RPPS[0], TABELA_RPPS[1], 8);
    }
    else if (s->regime_previdenciario == 2) {
        float base_funpresp = s->rem_bruta - AUX_ALIMENTACAO - TETO_RGPS - s->sessao_noturna - s->ad_ferias;
        if (base_funpresp > 0.0f) {
            s->cont_funpresp = base_funpresp * s->aliquota_funpresp;
        }
        s->cont_rgps = calcular_progressivo(s->rem_bruta - s->cont_funpresp, TABELA_RGPS[0], TABELA_RGPS[1], 4);
    }
    else if (s->regime_previdenciario == 3) {
        s->cont_rgps = calcular_progressivo(s->rem_bruta, TABELA_RGPS[0], TABELA_RGPS[1], 4);
    }

    float deducoes_ir = s->cont_funpresp + AUX_ALIMENTACAO + s->cont_rgps + s->cont_rpps;
    float base_ir = s->rem_bruta - deducoes_ir;
    s->imposto_renda = calcular_ir_progressivo(base_ir);

    s->rem_liquida = s->rem_bruta - s->cont_funpresp - s->cont_rgps - s->cont_rpps - s->pro_saude - s->imposto_renda;
}

/* --- Relatório --- */

void gerar_relatorio(Servidor *s) {
    char *classe;
    if (s->padrao <= 4) classe = "A";
    else if (s->padrao <= 8) classe = "B";
    else classe = "Especial";

    FILE* file = fopen("contracheque.txt", "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    printf("Cargo: Analista Legislativo | Classe: %s | Padrão: %d\n\n", classe, s->padrao);
    printf("(+) Vencimento básico ................... R$ %10.2f\n", VENC_BASICO[s->padrao-1]);
    printf("(+) Gratificação de Representação ....... R$ %10.2f\n", GRAT_REPRESENTACAO);
    printf("(+) Adicional de Especialização ......... R$ %10.2f\n", s->ad_especializacao);
    printf("(+) Grat. Atividade Legislativa ......... R$ %10.2f\n", GRAT_ATV_LEG[s->padrao-1]);
    printf("(+) VPI ................................. R$ %10.2f\n", VPI);
    printf("(+) Auxílio Alimentação ................. R$ %10.2f\n", AUX_ALIMENTACAO);
    if(s->ad_ferias > 0) printf("(+) Adicional de Férias ................. R$ %10.2f\n", s->ad_ferias);
    if(s->funcao_comissionada > 0) printf("(+) Função Comissionada ................. R$ %10.2f\n", s->funcao_comissionada);
    if(s->sessao_noturna > 0) printf("(+) Sessões Noturnas .................... R$ %10.2f\n", s->sessao_noturna);
    printf("------------------------------------------------------\n");
    printf("(=) REMUNERAÇÃO BRUTA ................... R$ %10.2f\n", s->rem_bruta);
    printf("------------------------------------------------------\n");
    printf("(-) Contribuição FUNPRESP ............... R$ %10.2f\n", s->cont_funpresp);
    printf("(-) Contribuição RGPS ................... R$ %10.2f\n", s->cont_rgps);
    printf("(-) Contribuição RPPS ................... R$ %10.2f\n", s->cont_rpps);
    printf("(-) Pró-Saúde ........................... R$ %10.2f\n", s->pro_saude);
    printf("(-) Imposto de Renda .................... R$ %10.2f\n", s->imposto_renda);
    printf("------------------------------------------------------\n");
    printf("(=) REMUNERAÇÃO LÍQUIDA ................. R$ %10.2f\n", s->rem_liquida);

    fprintf(file, "Cargo: Analista Legislativo | Classe: %s | Padrão: %d\n\n", classe, s->padrao);
    fprintf(file, "(+) Vencimento básico ................... R$ %10.2f\n", VENC_BASICO[s->padrao-1]);
    fprintf(file, "(+) Gratificação de Representação ....... R$ %10.2f\n", GRAT_REPRESENTACAO);
    fprintf(file, "(+) Adicional de Especialização ......... R$ %10.2f\n", s->ad_especializacao);
    fprintf(file, "(+) Grat. Atividade Legislativa ......... R$ %10.2f\n", GRAT_ATV_LEG[s->padrao-1]);
    fprintf(file, "(+) VPI ................................. R$ %10.2f\n", VPI);
    fprintf(file, "(+) Auxílio Alimentação ................. R$ %10.2f\n", AUX_ALIMENTACAO);
    if (s->ad_ferias > 0) fprintf(file, "(+) Adicional de Férias ................. R$ %10.2f\n", s->ad_ferias);
    if (s->funcao_comissionada > 0) fprintf(file, "(+) Função Comissionada ................. R$ %10.2f\n", s->funcao_comissionada);
    if (s->sessao_noturna > 0) fprintf(file, "(+) Sessões Noturnas .................... R$ %10.2f\n", s->sessao_noturna);
    fprintf(file, "(=) REMUNERAÇÃO BRUTA ................... R$ %10.2f\n", s->rem_bruta);
    fprintf(file, "(-) Contribuição FUNPRESP ............... R$ %10.2f\n", s->cont_funpresp);
    fprintf(file, "(-) Contribuição RGPS ................... R$ %10.2f\n", s->cont_rgps);
    fprintf(file, "(-) Contribuição RPPS ................... R$ %10.2f\n", s->cont_rpps);
    fprintf(file, "(-) Pró-Saúde ........................... R$ %10.2f\n", s->pro_saude);
    fprintf(file, "(-) Imposto de Renda .................... R$ %10.2f\n", s->imposto_renda);
    fprintf(file, "(=) REMUNERAÇÃO LÍQUIDA ................. R$ %10.2f\n", s->rem_liquida);

    fclose(file);
    printf("\nArquivo 'contracheque.txt' gerado com sucesso!\n");
}
