#ifndef NEGOCIO_H
#define NEGOCIO_H

#include "servidor.h"

float calcular_progressivo(float base_calculo, const float limites[], const float aliquotas[], int faixas);
float calcular_ir_progressivo(float base_calculo);
void calcular_rendimentos(Servidor *s);
void calcular_descontos(Servidor *s);

#endif
