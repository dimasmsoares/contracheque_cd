#ifndef SERVIDOR_H
#define SERVIDOR_H

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

#endif
