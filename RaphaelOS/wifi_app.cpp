#include "wifi_app.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoOTA.h>
#include <time.h>
#include "face.h"
#include "display.h"

static WebServer server(80);
static DNSServer dnsServer;
static Preferences prefs;

static bool modoAP = false;
static String ssid_salvo = "";
static String senha_salva = "";
static bool revelarIP = false;

const byte DNS_PORT = 53;

// HTML Responsivo e Estilizado do RaphaelOS
const char HTML_PAINEL[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RaphaelOS Dashboard</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, sans-serif; background: #121212; color: #fff; text-align: center; margin:0; padding:20px; }
    .card { background: #1e1e1e; padding: 20px; border-radius: 12px; max-width: 400px; margin: auto; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
    h1 { color: #00adb5; font-size: 24px; margin-bottom: 5px; }
    p { color: #888; font-size: 14px; }
    input, select { width: 90%; padding: 10px; margin: 8px 0; border-radius: 6px; border: 1px solid #333; background: #2a2a2a; color: #fff; }
    button { background: #00adb5; color: white; border: none; padding: 12px 20px; border-radius: 6px; cursor: pointer; font-weight: bold; width: 95%; margin-top: 10px; }
    button:hover { background: #008c93; }
    .btn-face { background: #393e46; margin: 4px; width: 45%; }
  </style>
</head>
<body>
  <div class="card">
    <h1>RaphaelOS</h1>
    <p>Painel de Configuracao</p>
    <form action="/salvar" method="POST">
      <h3>Conexao Wifi</h3>
      <input type="text" name="ssid" placeholder="Nome da rede (SSID)" required><br>
      <input type="password" name="senha" placeholder="Senha"><br>
      <button type="submit">Salvar e Conectar</button>
    </form>
    <hr style="border-color:#333; margin:20px 0;">
    <h3>Controle do Mascote</h3>
    <a href="/face?e=1"><button class="btn-face">Feliz</button></a>
    <a href="/face?e=2"><button class="btn-face">Triste</button></a>
    <a href="/face?e=4"><button class="btn-face">Dormindo</button></a>
    <a href="/face?e=0"><button class="btn-face">Neutro</button></a>
  </div>
</body>
</html>
)rawliteral";
void iniciarWiFi() {
  prefs.begin("raphaelos", false);
  ssid_salvo = prefs.getString("ssid", "DocaCyber_2.4");
  senha_salva = prefs.getString("senha", "Boladebasquet24");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_salvo.c_str(), senha_salva.c_str());

  int contador = 0;
  int maxContador = 15; // 15 tentativas x 500ms = 7.5s max

  while (WiFi.status() != WL_CONNECTED && contador < maxContador) {
    contador++;

    // Calcula a porcentagem e desenha a barra no OLED
    int progresso = (contador * 100) / maxContador;
    desenharProgressoWiFi(progresso);

    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    desenharProgressoWiFi(100); // Garante a barra preenchida em 100% ao conectar
    delay(300);

    modoAP = false;
    ArduinoOTA.setHostname("RaphaelOS-ESP32");
    ArduinoOTA.begin();
  } else {
    // Se não conectar, cria o Ponto de Acesso (AP)
    modoAP = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP("RaphaelOS-Config", "12345678");
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  }

  // Configuração das rotas do Servidor Web
  server.on("/", []() {
    server.send(200, "text/html", HTML_PAINEL);
  });

  server.on("/salvar", HTTP_POST, []() {
    String novoSSID = server.arg("ssid");
    String novaSenha = server.arg("senha");

    prefs.putString("ssid", novoSSID);
    prefs.putString("senha", novaSenha);

    server.send(200, "text/html", "<h2>Salvo com sucesso! Reiniciando o RaphaelOS...</h2>");
    delay(2000);
    ESP.restart();
  });

  server.on("/face", []() {
    int exp = server.arg("e").toInt();
    definirExpressao(static_cast<Expressao>(exp));
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.begin();
}

void processarWebManager() {
  if (modoAP) {
    dnsServer.processNextRequest();
  }
  server.handleClient();
}

bool estaEmModoAP() {
  return modoAP;
}

void checarOTA() {
  if (!modoAP) ArduinoOTA.handle();
}

bool estaConectadoWiFi() {
  return WiFi.status() == WL_CONNECTED;
}

void alternarRevelarIP() {
  revelarIP = !revelarIP;
}


String obterIPWiFi() {
  if (modoAP) return WiFi.softAPIP().toString();
  if (estaConectadoWiFi()) return WiFi.localIP().toString();
  return "0.0.0.0";
}
// Função que substitui o último octeto por *** se revelarIP for false
String obterIPWiFiFormatado() {
  if (!estaConectadoWiFi()) {
    return "0.0.0.0";
  }

  String ipCompleto = obterIPWiFi(); // Chama a sua função original

  if (revelarIP) {
    return ipCompleto; // Exibe o IP real (ex: 192.168.1.15)
  }

  // Oculta os últimos dígitos (ex: 192.168.1.***)
  int ultimoPonto = ipCompleto.lastIndexOf('.');
  if (ultimoPonto != -1) {
    return ipCompleto.substring(0, ultimoPonto + 1) + "***";
  }

  return ipCompleto;
}

int obterSinalWiFi() {
  return estaConectadoWiFi() ? WiFi.RSSI() : -100;
}

void sincronizarHorario() {
  if (estaConectadoWiFi()) {
    configTime(-10800, 0, "pool.ntp.org", "time.nist.gov");
  }
}

String obterHoraFormatada() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--:--:--";
  char horaStr[10];
  strftime(horaStr, sizeof(horaStr), "%H:%M:%S", &timeinfo);
  return String(horaStr);
}

String obterDataFormatada() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--/--/----";
  char dataStr[12];
  strftime(dataStr, sizeof(dataStr), "%d/%m/%Y", &timeinfo);
  return String(dataStr);
}