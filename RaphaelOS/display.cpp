#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "icons.h"
#include "menu.h"
#include "weather.h"
#include "wifi_app.h"
#include "system_status.h"
#include "face.h"

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_SSD1306 display(128, 64, &Wire, -1);

static int animFrame = 0;

void iniciarDisplay() {
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void desenharTelaConectando(String mensagem, int progresso) {
  display.clearDisplay();

  // ZONA AMARELA: Nome do Sistema
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.println("RaphaelOS");

  // ZONA AZUL: Progresso
  display.setTextSize(1);
  display.setCursor(10, 24);
  display.println(mensagem);

  display.drawRect(10, 42, 108, 10, SSD1306_WHITE);
  int larguraBarra = map(progresso, 0, 100, 0, 104);
  display.fillRect(12, 44, larguraBarra, 6, SSD1306_WHITE);

  display.display();
}

void exibirTelaSplash() {
  display.clearDisplay();
  
  // ZONA AMARELA
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("RaphaelOS");

  // ZONA AZUL
  display.setTextSize(1);
  display.setCursor(28, 22);
  display.println("Iniciando...");
  display.display();

  // Animação da barra na zona azul
  for (int i = 0; i <= 96; i += 4) {
    display.fillRect(16, 42, i, 6, SSD1306_WHITE);
    display.display();
    delay(50);
  }
}

void exibirTelaConectandoWiFi() {
  display.clearDisplay();
  
  // ZONA AMARELA
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("RaphaelOS");

  // ZONA AZUL
  display.setTextSize(1);
  display.setCursor(10, 24);
  display.println("Conectando Wi-Fi...");

  display.drawRect(14, 44, 100, 8, SSD1306_WHITE);
  display.display();
}

void desenharTela() {
  display.clearDisplay();
  
  int tela = obterTelaAtual();

  // TELA 0: MASCOTE (Tela cheia)
  if (tela == 0) {
    atualizarAnimacaoRosto();
    desenharRosto(display);
    display.display();
    return;
  }

  // =========================================================
  // 🟡 ZONA AMARELA (Y: 0 até 15) - CABEÇALHO FIXO
  // =========================================================
  display.setTextSize(1);
  display.setCursor(0, 4);
  display.print("RaphaelOS");

  // Bateria e Porcentagem na área amarela
  display.drawBitmap(72, 3, ICON_BATTERY, 16, 8, SSD1306_WHITE);
  display.setCursor(90, 4);
  display.print(String(obterPorcentagemBateria()) + "%");

  // Indicador do Wi-Fi (Ponto piscante amarelo no canto superior direito)
  animFrame = (animFrame + 1) % 10;
  if (estaConectadoWiFi() && animFrame > 4) {
    display.fillCircle(124, 7, 2, SSD1306_WHITE);
  }

  // Linha divisória amarela ajustada no limite exato da área amarela (Y = 15)
  display.drawLine(0, 15, 128, 15, SSD1306_WHITE);

  // =========================================================
  // 🔵 ZONA AZUL (Y: 16 até 63) - CONTEÚDO DAS TELAS
  // =========================================================

  if (tela == 1) { // TELA 1: RELÓGIO E DATA
    if (estaModoAutoAlternar()) {
      display.setCursor(0, 18);
      display.print("[AUTO]");
    }

    display.setTextSize(2);
    display.setCursor(16, 28);
    display.println(obterHoraFormatada());

    display.setTextSize(1);
    display.setCursor(30, 50);
    display.println(obterDataFormatada());
  } 
  else if (tela == 2) { // TELA 2: CLIMA
    display.setTextSize(1);
    display.setCursor(0, 18);
    display.println(obterCidade());

    display.drawBitmap(104, 22, ICON_SUN, 16, 16, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 30);
    display.println(obterTemperatura());

    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println(obterCondicao());
  } 
  else if (tela == 3) { // TELA 3: STATUS DO SISTEMA E REDE
    display.setTextSize(1);
    
    display.setCursor(0, 20);
    display.print("WiFi: ");
    display.println(estaConectadoWiFi() ? "Conectado" : "Desconectado");

    display.setCursor(0, 34);
    display.print("IP: ");
    display.println(obterIPWiFiFormatado());

    display.setCursor(0, 48);
    display.print("OTA: Ativo");
  }
  else if (tela == 4) { // TELA 4: GRÁFICO DA BATERIA
    display.setTextSize(1);
    
    // Título do gráfico na zona azul
    display.setCursor(0, 18);
    display.print("Bat: ");
    display.print(obterVoltagemBateria(), 2);
    display.print("V (");
    display.print(obterPorcentagemBateria());
    display.println("%)");

    // Moldura do gráfico ocupando o meio/fim da área azul
    display.drawRect(4, 29, 120, 32, SSD1306_WHITE);

    float* hist = obterHistoricoBateria();

    for (int i = 0; i < 59; i++) {
      float v1 = hist[i];
      float v2 = hist[i + 1];

      if (v1 == 0.0 || v2 == 0.0) continue;

      // Mapeia 3.0V ~ 4.2V para o interior da moldura azul (Y: 59 até Y: 31)
      int y1 = map(v1 * 100, 300, 420, 59, 31);
      int y2 = map(v2 * 100, 300, 420, 59, 31);

      y1 = constrain(y1, 31, 59);
      y2 = constrain(y2, 31, 59);

      int x1 = 6 + (i * 1.9);
      int x2 = 6 + ((i + 1) * 1.9);

      display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }
  }

  display.display();
}

void desenharProgressoWiFi(int progressoPercentual) {
  display.clearDisplay();
  
  // ZONA AMARELA
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("RaphaelOS");

  // ZONA AZUL
  display.setTextSize(1);
  display.setCursor(10, 22);
  display.println("Conectando Wi-Fi...");

  // Moldura da barra na zona azul
  display.drawRect(14, 42, 100, 10, SSD1306_WHITE);

  int larguraPreenchimento = map(progressoPercentual, 0, 100, 0, 96);
  if (larguraPreenchimento > 0) {
    display.fillRect(16, 44, larguraPreenchimento, 6, SSD1306_WHITE);
  }
  display.display();
}