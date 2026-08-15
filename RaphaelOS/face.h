#ifndef FACE_H
#define FACE_H

#include <Adafruit_SSD1306.h>

enum Expressao {
  NEUTRO,
  FELIZ,
  TRISTE,
  PISCANDO,
  DORMINDO,
  OLHANDO_ESQUERDA,
  OLHANDO_DIREITA
};

void definirExpressao(Expressao novaExpressao);
void proximaExpressao();
void atualizarAnimacaoRosto(); // ⬅️ Atualização de movimentos automáticos
void desenharRosto(Adafruit_SSD1306 &display);

#endif