#ifndef CONSTANTES_H
#define CONSTANTES_H

// Tabelas Salariais (Atualizado em: Dez/2023)
static const float VENC_BASICO[10] = {
    5739.22, 6041.29, 6293.01, 6555.23, 7365.42, 
    7672.32, 7991.98, 8324.99, 8903.77, 9274.73
};

static const float GRAT_ATV_LEG[10] = {
    7460.99, 7853.68, 8180.91, 8521.80, 9575.05, 
    9974.02, 10389.57, 10822.49, 11574.90, 12057.15
};

static const float GRAT_REPRESENTACAO = 16202.70;
static const float VPI = 59.87;
static const float AUX_ALIMENTACAO = 1784.42;

// Tabela IR
static const float TABELA_IR[3][5] = {
    {2428.80, 2826.65, 3751.05, 4664.68, 1000000.00},
    {0.0, 0.075, 0.15, 0.225, 0.275}, 
    {0.0, 169.44, 381.44, 662.77, 896.00}
};

// Tabela RGPS
static const float TABELA_RGPS[2][4] = {
    {1621.00, 2902.84, 4354.27, 8475.55},
    {0.075, 0.09, 0.12, 0.14}
};

static const float TETO_RGPS = 8475.55;

// Tabela RPPS
static const float TABELA_RPPS[2][8] = {
    {1621.00, 2902.84, 4354.27, 8475.55, 14514.30, 29028.57, 56605.73, 100000.00}, 
    {0.075, 0.09, 0.12, 0.14, 0.145, 0.165, 0.19, 0.22}
};

// Tabela Pró-Saúde
static const float TAB_PRO_SAUDE[2][10] = {
    {19, 23, 28, 33, 38, 43, 48, 53, 58, 150}, 
    {332.00, 365.20, 398.40, 431.60, 498.00, 564.40, 630.80, 697.20, 763.60, 830.00}
};

// Tabela Funções Comissionadas
static const float TAB_FC[6] = {4770.89, 6542.93, 9132.84, 10359.65, 11177.51, 12854.13};

#endif