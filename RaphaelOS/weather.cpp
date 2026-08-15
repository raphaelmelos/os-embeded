#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "weather.h"
#include "wifi_app.h"

const String LAT = "-30.08986702777224";
const String LON = "-51.09361593408104";

// Nome da sua cidade configurado manualmente
const String NOME_CIDADE = "Porto Alegre"; // ⬅️ Troque pelo nome que desejar (ex: "Porto Alegre", "Viamao")

static String temperatura = "-- C";
static String condicao = "Conectando...";

unsigned long ultimoTempoAtualizacao = 0;
const unsigned long INTERVALO_ATUALIZACAO = 300000; // 5 minutos

String traduzirCodigoClima(int code) {
  if (code == 0) return "Ceu Limpo";
  if (code >= 1 && code <= 3) return "Parcial. Nublado";
  if (code >= 45 && code <= 48) return "Nevoeiro";
  if (code >= 51 && code <= 67) return "Garoa / Chuva";
  if (code >= 80 && code <= 82) return "Pancadas Chuva";
  if (code >= 95) return "Tempestade";
  return "Nublado";
}

void iniciarWeather() {
  condicao = "Conectando WiFi";
}

void atualizarWeather() {
  if (!estaConectadoWiFi()) {
    condicao = "Sem WiFi...";
    return;
  }

  if (millis() - ultimoTempoAtualizacao < INTERVALO_ATUALIZACAO && temperatura != "-- C") {
    return;
  }

  ultimoTempoAtualizacao = millis();

  HTTPClient http;
  String url = "http://api.open-meteo.com/v1/forecast?latitude=" + LAT + "&longitude=" + LON + "&current_weather=true";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      float temp = doc["current_weather"]["temperature"];
      int weatherCode = doc["current_weather"]["weathercode"];

      temperatura = String((int)round(temp)) + " C";
      condicao = traduzirCodigoClima(weatherCode);
    } else {
      condicao = "Erro no JSON";
    }
  } else {
    condicao = "Erro HTTP: " + String(httpCode);
  }

  http.end();
}

String obterTemperatura() {
  return temperatura;
}

String obterCondicao() {
  return condicao;
}

// ⬅️ Função que retorna o nome da sua cidade
String obterCidade() {
  return NOME_CIDADE;
}