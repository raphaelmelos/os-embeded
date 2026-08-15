#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

void iniciarWeather();
void atualizarWeather();
String obterTemperatura();
String obterCondicao();
String obterCidade(); 

#endif