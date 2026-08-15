#include "menu.h"
#include "face.h"
#include "wifi_app.h"

#define BOTAO_PIN 4

static int telaAtual = 0;
const int TOTAL_TELAS = 5;

static bool modoAutoAlternar = true;
unsigned long ultimoTrocaAuto = 0;
unsigned long ultimoInteracaoUsuario = 0;

const unsigned long TEMPO_AUTO_TROCA = 5000;
const unsigned long TEMPO_VOLTAR_AUTO = 15000;

unsigned long tempoPressionado = 0;
bool botaoPressionado = false;
const unsigned long TEMPO_PRESSIONADO_LONGO = 500;

void iniciarMenu() {
  pinMode(BOTAO_PIN, INPUT_PULLUP);
}

bool estaModoAutoAlternar() {
  return modoAutoAlternar;
}

void checarBotao() {
  unsigned long agora = millis();

  if (modoAutoAlternar) {
    if (agora - ultimoTrocaAuto > TEMPO_AUTO_TROCA) {
      telaAtual = (telaAtual == 0) ? 1 : 0;
      ultimoTrocaAuto = agora;
    }
  } else {
    if (agora - ultimoInteracaoUsuario > TEMPO_VOLTAR_AUTO) {
      modoAutoAlternar = true;
      telaAtual = 0;
      ultimoTrocaAuto = agora;
    }
  }

  int estadoBotao = digitalRead(BOTAO_PIN);

  if (estadoBotao == LOW && !botaoPressionado) {
    botaoPressionado = true;
    tempoPressionado = agora;
  }

  if (estadoBotao == HIGH && botaoPressionado) {
    unsigned long duracaoClique = agora - tempoPressionado;
    botaoPressionado = false;

    if (duracaoClique > 50) { // Debounce
      ultimoInteracaoUsuario = agora;
      modoAutoAlternar = false;

      // Tela 0: Mascote
      if (telaAtual == 0) {
        if (duracaoClique >= TEMPO_PRESSIONADO_LONGO) {
          telaAtual = (telaAtual + 1) % TOTAL_TELAS;
        } else {
          proximaExpressao();
        }
      } 
      // Tela 3: Status / Wi-Fi
      else if (telaAtual == 3) {
        if (duracaoClique >= TEMPO_PRESSIONADO_LONGO) {
          alternarRevelarIP(); // ⬅️ Manter pressionado revela/esconde o IP
        } else {
          telaAtual = (telaAtual + 1) % TOTAL_TELAS; // ⬅️ Clique curto avança a tela
        }
      } 
      // Demais telas
      else {
        telaAtual = (telaAtual + 1) % TOTAL_TELAS;
      }
    }
  }
}

int obterTelaAtual() {
  return telaAtual;
}