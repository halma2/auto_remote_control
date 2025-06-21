#ifndef LED_H
#define LED_H

// LED GPIO lábak
#define RED_PIN    21
#define GREEN_PIN  22
#define BLUE_PIN   23

// Inicializálás
void ledInit();

// Szín beállítása
// true = világít, false = nem világít
void ledSetColor(bool red, bool green, bool blue);

#endif