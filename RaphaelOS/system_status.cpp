#include "system_status.h"

#define PINO_ADC_BATERIA 34

// Histórico para o gráfico (60 pontos cobrem bem a largura da tela OLED)
static float historicoBateria[60];
static float voltagemAtual = 0.0;
static unsigned long ultimaLeitura = 0;

void iniciarSistemaStatus() {
  analogReadResolution(12); // Resolução de 12 bits (0 a 4095)
  
  // Preenche o histórico inicial com 0.0
  for (int i = 0; i < 60; i++) {
    historicoBateria[i] = 0.0;
  }
}

void atualizarLeituraBateria() {
  unsigned long agora = millis();
  
  // Atualiza a leitura a cada 1 segundo (1000ms)
  if (agora - ultimaLeitura >= 1000 || ultimaLeitura == 0) {
    ultimaLeitura = agora;

    int leituraADC = analogRead(PINO_ADC_BATERIA);
    // Fórmula para divisor de tensão 100k/100k (fator x2)
    voltagemAtual = (leituraADC / 4095.0) * 3.3 * 2.0;

    // Desloca o histórico para a esquerda (efeito scroll do gráfico)
    for (int i = 0; i < 59; i++) {
      historicoBateria[i] = historicoBateria[i + 1];
    }
    historicoBateria[59] = voltagemAtual;
  }
}

float obterVoltagemBateria() {
  return voltagemAtual;
}

int obterPorcentagemBateria() {
  int pct = map(voltagemAtual * 100, 330, 420, 0, 100);
  if (pct > 100) pct = 100;
  if (pct < 0) pct = 0;
  return pct;
}

float* obterHistoricoBateria() {
  return historicoBateria;
}