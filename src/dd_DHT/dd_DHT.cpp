#include "dd_dht.h"
#include <Arduino.h>

#define DHTPIN 14

void dd_dht_setup(void) {
    pinMode(DHTPIN, INPUT_PULLUP);
}

static bool read_dht_raw(uint8_t data[5]) {
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // 1. Semnalul de Start trimis de MCU
    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, LOW);
    delay(18); 
    digitalWrite(DHTPIN, HIGH);
    delayMicroseconds(40);
    pinMode(DHTPIN, INPUT_PULLUP);

    // 2. Așteptare răspuns senzor (Acknowledge)
    uint32_t timeout = 10000;
    while(digitalRead(DHTPIN) == HIGH) { if (--timeout == 0) return false; }
    timeout = 10000;
    while(digitalRead(DHTPIN) == LOW)  { if (--timeout == 0) return false; }
    timeout = 10000;
    while(digitalRead(DHTPIN) == HIGH) { if (--timeout == 0) return false; }

    // 3. Citirea celor 40 de biți bazată pe timp real (micros)
    noInterrupts();
    for (int i = 0; i < 40; i++) {
        timeout = 10000;
        while(digitalRead(DHTPIN) == LOW) { if (--timeout == 0) break; }

        uint32_t t_start = micros();
        
        timeout = 10000;
        while(digitalRead(DHTPIN) == HIGH) { if (--timeout == 0) break; }
        
        uint32_t t_duration = micros() - t_start;

        data[i / 8] <<= 1;
        // Un impuls HIGH mai lung de 40 microsecunde reprezintă bitul '1'
        if (t_duration > 40) {
            data[i / 8] |= 1;
        }
    }
    interrupts();

    // 4. Verificare Checksum
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return true;
    }

    return false;
}

float dd_dht_read_temperature(void) {
    uint8_t data[5];
    if (!read_dht_raw(data)) return -999.0;

    float t = ((data[2] & 0x7F) << 8 | data[3]) / 10.0;
    if (data[2] & 0x80) t = -t; 
    return t;
}

float dd_dht_read_humidity(void) {
    uint8_t data[5];
    if (!read_dht_raw(data)) return -999.0;

    return ((data[0] << 8) | data[1]) / 10.0;
}