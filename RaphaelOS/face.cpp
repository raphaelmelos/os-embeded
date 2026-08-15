#include "face.h"

static Expressao expressaoAtual = NEUTRO;
static unsigned long ultimoTrocaMovimento = 0;
static unsigned long tempoProximoMovimento = 3000;

static unsigned long ultimoPiscar = 0;
static bool estaPiscando = false;

void definirExpressao(Expressao novaExpressao) {
  expressaoAtual = novaExpressao;
}

void proximaExpressao() {
  // Alterna manualmente se o usuário apertar o botão
  if (expressaoAtual == NEUTRO) expressaoAtual = FELIZ;
  else if (expressaoAtual == FELIZ) expressaoAtual = TRISTE;
  else if (expressaoAtual == TRISTE) expressaoAtual = DORMINDO;
  else expressaoAtual = NEUTRO;
}

// ⬅️ LÓGICA DE MOVIMENTOS AUTOMÁTICOS
void atualizarAnimacaoRosto() {
  unsigned long agora = millis();

  // Piscar de olhos natural a cada 3.5 segundos
  if (agora - ultimoPiscar > 3500) {
    estaPiscando = true;
    if (agora - ultimoPiscar > 3650) { // Fecha por 150ms
      estaPiscando = false;
      ultimoPiscar = agora;
    }
  }

  // Sorteia movimentos (Olhar para Lados / Ficar Feliz / Neutro) a cada tempo variado
  if (agora - ultimoTrocaMovimento > tempoProximoMovimento) {
    ultimoTrocaMovimento = agora;
    tempoProximoMovimento = random(2000, 5000); // Muda entre 2 e 5 segundos

    int sorteio = random(0, 100);

    if (sorteio < 40) {
      expressaoAtual = NEUTRO;
    } else if (sorteio < 60) {
      expressaoAtual = OLHANDO_ESQUERDA;
    } else if (sorteio < 80) {
      expressaoAtual = OLHANDO_DIREITA;
    } else if (sorteio < 92) {
      expressaoAtual = FELIZ;
    } else {
      expressaoAtual = PISCANDO;
    }
  }
}

void desenharRosto(Adafruit_SSD1306 &display) {
  // --- EXPRESSÃO: PISCANDO / FECHADO ---
  if (estaPiscando || expressaoAtual == PISCANDO || expressaoAtual == DORMINDO) {
    display.fillRect(30, 32, 22, 4, SSD1306_WHITE);
    display.fillRect(76, 32, 22, 4, SSD1306_WHITE);
    return;
  }

  // --- EXPRESSÃO: FELIZ (^ ^) ---
  if (expressaoAtual == FELIZ) {
    display.drawCircle(41, 38, 12, SSD1306_WHITE);
    display.fillRect(26, 38, 30, 15, SSD1306_BLACK);

    display.drawCircle(87, 38, 12, SSD1306_WHITE);
    display.fillRect(72, 38, 30, 15, SSD1306_BLACK);

    display.drawCircle(64, 45, 5, SSD1306_WHITE);
    display.fillRect(55, 35, 18, 10, SSD1306_BLACK);
    return;
  }

  // --- EXPRESSÃO: TRISTE ---
  if (expressaoAtual == TRISTE) {
    display.fillCircle(41, 32, 10, SSD1306_WHITE);
    display.fillCircle(87, 32, 10, SSD1306_WHITE);
    display.drawLine(26, 18, 49, 24, SSD1306_WHITE);
    display.drawLine(102, 18, 79, 24, SSD1306_WHITE);
    return;
  }

  // --- EXPRESSÃO: NEUTRA E OLHARES DIREÇÃO ---
  // Moldura dos olhos
  display.fillRoundRect(30, 20, 22, 26, 6, SSD1306_WHITE);
  display.fillRoundRect(76, 20, 22, 26, 6, SSD1306_WHITE);

  int offsetPupilaX = 0;
  if (expressaoAtual == OLHANDO_ESQUERDA) offsetPupilaX = -4;
  if (expressaoAtual == OLHANDO_DIREITA) offsetPupilaX = 4;

  // Pupilas dinâmicas que se movem!
  display.fillCircle(36 + offsetPupilaX, 26, 4, SSD1306_BLACK);
  display.fillCircle(82 + offsetPupilaX, 26, 4, SSD1306_BLACK);
}