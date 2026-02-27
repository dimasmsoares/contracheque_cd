#include "constantes.h"
#include "negocio.h"

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
    } else if (s->regime_previdenciario == 2) {
        float base_funpresp = s->rem_bruta - AUX_ALIMENTACAO - TETO_RGPS - s->sessao_noturna - s->ad_ferias;
        if (base_funpresp > 0.0f) {
            s->cont_funpresp = base_funpresp * s->aliquota_funpresp;
        }
        s->cont_rgps = calcular_progressivo(s->rem_bruta - s->cont_funpresp, TABELA_RGPS[0], TABELA_RGPS[1], 4);
    } else {
        s->cont_rgps = calcular_progressivo(s->rem_bruta, TABELA_RGPS[0], TABELA_RGPS[1], 4);
    }

    float deducoes_ir = s->cont_funpresp + AUX_ALIMENTACAO + s->cont_rgps + s->cont_rpps;
    float base_ir = s->rem_bruta - deducoes_ir;
    s->imposto_renda = calcular_ir_progressivo(base_ir);

    s->rem_liquida = s->rem_bruta - s->cont_funpresp - s->cont_rgps - s->cont_rpps - s->pro_saude - s->imposto_renda;
}
