/******************************Headers*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // Para a função toupper()
#include "constantes.h"

/******************************Estruturas**************************************/
typedef struct {
    int padrao;
    float rem_bruta;
    float rem_liquida;
    
    // Adicionais
    float ad_especializacao;
    float ad_ferias;
    float funcao_comissionada;
    
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
float calcular_progressivo(float base_calculo, const float tabela[][8], int colunas); // Genérico para RPPS
float calcular_rgps_progressivo(float base_calculo); 
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

    // 2. Especialização (Lógica simplificada para clareza)
    float perc_doutorado = (ler_confirmacao("Possui doutorado?") == 'S') ? 10.0 : 0.0;
    float perc_mestrado = (ler_confirmacao("Possui mestrado?") == 'S') ? 8.0 : 0.0;
    
    printf("\nPossui especializações?\n1 - Não\n2 - 1ª Esp.\n3 - 1ª e 2ª Esp.\n");
    int op_esp = ler_inteiro("Sua opção: ", 1, 3);
    float perc_pos = (op_esp == 2) ? 6.0 : (op_esp == 3 ? 9.0 : 0.0);

    float perc_grad = (ler_confirmacao("Possui 2ª graduação?") == 'S') ? 21.0 : 15.0; // Assume base + 2ª grad ou só grad base? Mantive a lógica original, mas cuidado aqui.

    float total_perc = perc_doutorado + perc_mestrado + perc_pos + perc_grad;
    if (total_perc > 30.0) total_perc = 30.0;
    
    // Calcula valor absoluto baseado no topo da carreira (índice 9)
    s->ad_especializacao = VENC_BASICO[9] * (total_perc / 100.0);

    // 3. Adicional de Férias
    if (ler_confirmacao("Adicional de Férias?") == 'S') {
        // Marcamos com -1 temporariamente para saber que precisa calcular depois que tivermos o bruto
        s->ad_ferias = -1.0; 
    } else {
        s->ad_ferias = 0.0;
    }

    // 4. Função Comissionada
    printf("\nFunção comissionada (0 p/ não possuir, 1-6 para FC):\n");
    int op_fc = ler_inteiro("Sua opção: ", 0, 6);
    s->funcao_comissionada = (op_fc == 0) ? 0.0 : TAB_FC[op_fc - 1];

    // 5. Previdência (Apenas coleta o tipo por enquanto, cálculo vem depois)
    printf("\nRegime Previdenciário:\n1 - RPPS\n2 - RGPS + FUNPRESP\n3 - RGPS\n");
    int op_prev = ler_inteiro("Sua opção: ", 1, 3);
    
    // Armazenamos lógica temporária em variáveis da struct para uso no calculo
    // Hack: Usando cont_rpps como flag temporária do tipo
    s->cont_rpps = (float)op_prev; 
    
    if (op_prev == 2) {
        printf("\nPercentual FUNPRESP:\n1 - 7.5%%\n2 - 8.0%%\n3 - 8.5%%\n");
        int op_funp = ler_inteiro("Sua opção: ", 1, 3);
        if (op_funp == 1) s->cont_funpresp = 0.075;
        else if (op_funp == 2) s->cont_funpresp = 0.080;
        else s->cont_funpresp = 0.085;
    } else {
        s->cont_funpresp = 0.0; // Flag 0 significa sem funpresp ou percentual 0
    }

    // 6. Pró-Saúde
    if (ler_confirmacao("Aderiu ao Pró-Saúde?") == 'S') {
        int idade = ler_inteiro("Digite sua idade: ", 0, 150);
        // Busca na tabela sem goto
        s->pro_saude = TAB_PRO_SAUDE[1][9]; // Default para maior idade
        for (int i = 0; i < 10; i++) {
            if (idade <= TAB_PRO_SAUDE[0][i]) {
                s->pro_saude = TAB_PRO_SAUDE[1][i];
                break;
            }
        }
    } else {
        s->pro_saude = 0.0;
    }
}

/* --- Lógica de Negócio: Cálculos --- */

// Função auxiliar para cálculo progressivo genérico (RPPS e IR seguem lógicas similares de faixas)
float calcular_progressivo_rpps(float base) {
    float imposto = 0.0;
    for (int i = 0; i < 8; i++) {
        float limite_inferior = (i == 0) ? 0 : TABELA_RPPS[0][i-1];
        float limite_superior = TABELA_RPPS[0][i];
        float aliquota = TABELA_RPPS[1][i];

        if (base > limite_superior) {
            imposto += (limite_superior - limite_inferior) * aliquota;
        } else {
            imposto += (base - limite_inferior) * aliquota;
            break;
        }
    }
    return imposto;
}

float calcular_rgps_progressivo(float base) {
    float imposto = 0.0;
    for (int i = 0; i < 4; i++) {
        float limite_inferior = (i == 0) ? 0 : TABELA_RGPS[0][i-1];
        float limite_superior = TABELA_RGPS[0][i];
        float aliquota = TABELA_RGPS[1][i];

        if (base > limite_superior) {
            imposto += (limite_superior - limite_inferior) * aliquota;
        } else {
            imposto += (base - limite_inferior) * aliquota;
            break;
        }
    }
    return imposto;
}

float calcular_ir_progressivo(float base) {
    float imposto = 0.0;
    for (int i = 0; i < 5; i++) {
        // Nota: A lógica original usava tabela progressiva manual. 
        // A forma mais comum é (Base * Aliquota) - Dedução.
        // Vou manter a lógica original progressiva "faixa a faixa" que você implementou, pois está correta matematicamente.
        float limite_inferior = (i == 0) ? 0 : TABELA_IR[0][i-1];
        float limite_superior = TABELA_IR[0][i];
        float aliquota = TABELA_IR[1][i];

        if (base > limite_superior) {
             imposto += (limite_superior - limite_inferior) * aliquota;
        } else {
             imposto += (base - limite_inferior) * aliquota;
             break;
        }
    }
    return imposto;
}

void calcular_rendimentos(Servidor *s) {
    // Soma inicial básica
    float soma_base = VENC_BASICO[s->padrao - 1] + 
                      GRAT_REPRESENTACAO + 
                      s->ad_especializacao + 
                      GRAT_ATV_LEG[s->padrao - 1] + 
                      VPI + 
                      AUX_ALIMENTACAO + 
                      s->funcao_comissionada;

    // Se a flag de férias estava ativa, calcula agora
    if (s->ad_ferias < 0) {
        // Nota: Adicional de férias incide sobre o total da remuneração? 
        // Normalmente sim (exceto auxílios indenizatórios como Alimentação).
        // Mantendo a lógica do seu código original (rem_bruta/3), mas ajustando para não ser circular
        // Vamos assumir que incide sobre tudo (simplificação do seu código original)
        s->ad_ferias = soma_base / 3.0; 
    }

    s->rem_bruta = soma_base + s->ad_ferias;
}

void calcular_descontos(Servidor *s) {
    int tipo_prev = (int)s->cont_rpps; // Recupera a flag salva
    float aliquota_funp = s->cont_funpresp; // Recupera percentual
    
    // Resetando valores para cálculo limpo
    s->cont_rpps = 0;
    s->cont_rgps = 0;
    s->cont_funpresp = 0;

    // 1. Previdência
    if (tipo_prev == 1) { // RPPS
        s->cont_rpps = calcular_progressivo_rpps(s->rem_bruta);
    } 
    else if (tipo_prev == 2) { // RGPS + FUNPRESP
        // Funpresp incide sobre o que excede o teto
        float base_funpresp = s->rem_bruta - AUX_ALIMENTACAO - TETO_RGPS;
        if (base_funpresp > 0) {
            s->cont_funpresp = base_funpresp * aliquota_funp;
        }
        // RGPS usa base total menos a contribuição funpresp (conforme seu código original)
        s->cont_rgps = calcular_rgps_progressivo(s->rem_bruta - s->cont_funpresp); 
    } 
    else if (tipo_prev == 3) { // Apenas RGPS
        s->cont_rgps = calcular_rgps_progressivo(s->rem_bruta);
    }

    // 2. Imposto de Renda
    float deducoes_ir = s->cont_funpresp + AUX_ALIMENTACAO + s->cont_rgps + s->cont_rpps;
    // Adiciona dedução por dependente se houvesse.
    // Nota: IR abate a previdência oficial e complementar.
    
    s->imposto_renda = calcular_ir_progressivo(s->rem_bruta - deducoes_ir);

    // 3. Líquido Final
    s->rem_liquida = s->rem_bruta - s->cont_funpresp - s->cont_rgps - s->cont_rpps - s->pro_saude - s->imposto_renda;
}

/* --- Relatório --- */

void gerar_relatorio(Servidor *s) {
    // Define a string da classe
    char *classe;
    if (s->padrao <= 4) classe = "A";
    else if (s->padrao <= 8) classe = "B";
    else classe = "Especial";

    // Cria/Abre arquivo
    FILE* file = fopen("contracheque.txt", "w");
    if (file == NULL) {
        printf("Erro ao criar arquivo!\n");
        return;
    }

    // Função interna (lambda-like) ou macro para imprimir em dois lugares
    // Para simplificar em C ANSI, fazemos prints sequenciais ou usamos fprintf com stdout vs file
    // Vamos imprimir primeiro na tela, depois no arquivo, repetindo a lógica (simples e didático)

    // Cabeçalho Tela
    printf("Cargo: Analista Legislativo | Classe: %s | Padrão: %d\n\n", classe, s->padrao);
    printf("(+) Vencimento básico ................... R$ %10.2f\n", VENC_BASICO[s->padrao-1]);
    printf("(+) Gratificação de Representação ....... R$ %10.2f\n", GRAT_REPRESENTACAO);
    printf("(+) Adicional de Especialização ......... R$ %10.2f\n", s->ad_especializacao);
    printf("(+) Grat. Atividade Legislativa ......... R$ %10.2f\n", GRAT_ATV_LEG[s->padrao-1]);
    printf("(+) VPI ................................. R$ %10.2f\n", VPI);
    printf("(+) Auxílio Alimentação ................. R$ %10.2f\n", AUX_ALIMENTACAO);
    if(s->ad_ferias > 0) printf("(+) Adicional de Férias ................. R$ %10.2f\n", s->ad_ferias);
    if(s->funcao_comissionada > 0) printf("(+) Função Comissionada ................. R$ %10.2f\n", s->funcao_comissionada);
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

    // Gravação no Arquivo (Repetição controlada para garantir formatação)
    fprintf(file, "Cargo: Analista Legislativo | Classe: %s | Padrão: %d\n\n", classe, s->padrao);
    fprintf(file, "(+) Vencimento básico ................... R$ %10.2f\n", VENC_BASICO[s->padrao-1]);
    fprintf(file, "(+) Gratificação de Representação ....... R$ %10.2f\n", GRAT_REPRESENTACAO);
    fprintf(file, "(+) Adicional de Especialização ......... R$ %10.2f\n", s->ad_especializacao);
    fprintf(file, "(+) Grat. Atividade Legislativa ......... R$ %10.2f\n", GRAT_ATV_LEG[s->padrao-1]);
    fprintf(file, "(+) VPI ................................. R$ %10.2f\n", VPI);
    fprintf(file, "(+) Auxílio Alimentação ................. R$ %10.2f\n", AUX_ALIMENTACAO);
    fprintf(file, "(+) Adicional de Férias ................. R$ %10.2f\n", s->ad_ferias);
    fprintf(file, "(+) Função Comissionada ................. R$ %10.2f\n", s->funcao_comissionada);
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