#include <stdio.h>

#include "constantes.h"
#include "relatorio.h"

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
    if (s->ad_ferias > 0) printf("(+) Adicional de Férias ................. R$ %10.2f\n", s->ad_ferias);
    if (s->funcao_comissionada > 0) printf("(+) Função Comissionada ................. R$ %10.2f\n", s->funcao_comissionada);
    if (s->sessao_noturna > 0) printf("(+) Sessões Noturnas .................... R$ %10.2f\n", s->sessao_noturna);
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
