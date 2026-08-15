#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <Arduino.h>

void iniciarSistemaStatus();
void atualizarLeituraBateria();
float obterVoltagemBateria();
int obterPorcentagemBateria();
float* obterHistoricoBateria();

#endif