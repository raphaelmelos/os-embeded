#ifndef WIFI_APP_H
#define WIFI_APP_H

#include <Arduino.h>

void iniciarWiFi();
void checarOTA();
bool estaConectadoWiFi();
String obterIPWiFi();
int obterSinalWiFi();
void sincronizarHorario();
String obterHoraFormatada();
String obterDataFormatada();
void processarWebManager(); // ⬅️ Trata o servidor Web e o Portal
bool estaEmModoAP();        // ⬅️ Retorna true se estiver no modo Ponto de Acesso
void alternarRevelarIP();
String obterIPWiFiFormatado(); 
#endif