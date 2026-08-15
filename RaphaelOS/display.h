#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void iniciarDisplay();
void desenharTela();

// ⬅️ Adicione estas duas linhas abaixo:
void exibirTelaSplash();
void exibirTelaConectandoWiFi();
void desenharProgressoWiFi(int progressoPercentual);

#endif