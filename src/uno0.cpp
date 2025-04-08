#include <Arduino.h>
#include "fakeDMA.h"

#define pinANALOG A5 // Configura o pino de leitura
uint32_t timeStampMS = 0;

void setup() {
    Serial.begin(9600);
    fakeDMA_init(pinANALOG);  // usar pino A0
    fakeDMA_start();
}

void loop() {
    if (fakeDMA_available()) {
        uint16_t value = fakeDMA_read();
        Serial.print(">graf:");
        Serial.print(timeStampMS++);
        Serial.print(":");
        Serial.print(analogRead(pinANALOG));
        Serial.println("|g");
    }

    // loop livre para outras tarefas
}