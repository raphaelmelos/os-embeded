#include "display.h"
#include "wifi_app.h"
#include "weather.h"
#include "menu.h"
#include "system_status.h"

bool horarioSincronizado = false;

void setup() {
  Serial.begin(115200);

  iniciarDisplay();

  // Exibe a tela com a animação da barra de carregamento
  exibirTelaSplash(); 

  iniciarSistemaStatus();
  iniciarMenu();

  // Exibe mensagem de busca pelo Wi-Fi
  exibirTelaConectandoWiFi();
  iniciarWiFi();
}

void loop() {
  processarWebManager();
  checarOTA();
  checarBotao();

  atualizarLeituraBateria();

  if (estaConectadoWiFi() && !horarioSincronizado) {
    sincronizarHorario();
    horarioSincronizado = true;
  }

  atualizarWeather();
  desenharTela();
  delay(50);
}