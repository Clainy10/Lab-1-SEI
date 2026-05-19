#include "app_lab_5_2.h"
#include "dd_dht/dd_dht.h"
#include "dd_l298/dd_l298.h"
#include "serial_stdio/serial_stdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

static float temp_curenta = 0.0, set_point = 25.0;
static float eroare_curenta = 0.0, eroare_anterioara = 0.0, suma_integrala = 0.0;
static float t_P = 0.0, t_I = 0.0, t_D = 0.0;
static int16_t motor_power = 0;
static unsigned long timp_anterior_dht = 0, timp_anterior_pid = 0;
static char cmd_buf[16];
static int cmd_idx = 0;

#define K_P 15.0f
#define K_I 0.5f
#define K_D 5.0f

static void executa_pid(unsigned long t_curent) {
    if (temp_curenta == -999.0) { dd_l298_stop(); return; }

    float dt = (t_curent - timp_anterior_pid) / 1000.0f;
    if (dt <= 0.0f) dt = 1.0f; 

    eroare_curenta = set_point - temp_curenta;
    suma_integrala += eroare_curenta * dt;
    
    if (suma_integrala > (100.0f / K_I)) suma_integrala = 100.0f / K_I;
    else if (suma_integrala < 0.0f) suma_integrala = 0.0f;

    t_P = K_P * eroare_curenta;
    t_I = K_I * suma_integrala;
    t_D = K_D * ((eroare_curenta - eroare_anterioara) / dt);

    float output = t_P + t_I + t_D;
    
    if (output > 100.0f) motor_power = 100;
    else if (output < 0.0f) motor_power = 0;
    else motor_power = (int16_t)output;

    dd_l298_set_power(motor_power);

    eroare_anterioara = eroare_curenta;
    timp_anterior_pid = t_curent;
}

static void afiseaza_date() {
    printf("\r\n--- PID DATA ---\r\n");
    printf("SetPoint: %d\r\n", (int)set_point);
    printf("Temp:     %d\r\n", (int)temp_curenta);
    printf("Eroare:   %d\r\n", (int)eroare_curenta);
    printf("P:%d I:%d D:%d\r\n", (int)t_P, (int)t_I, (int)t_D);
    printf("Motor:    %d\r\n", motor_power);
    printf("----------------\r\n");
}

void app_lab_5_2_setup(void) {
    serial_stdio_setup(); 
    dd_l298_setup();    
    dd_dht_setup();  
    delay(2000); 
    
    cmd_idx = 0;
    timp_anterior_pid = millis();
    temp_curenta = dd_dht_read_temperature();
}

void app_lab_5_2_run(void) {
    unsigned long t_curent = millis();

    // Citire comanda "set [valoare]"
    if (Serial.available() > 0) {
        char key;
        if (scanf("%c", &key) == 1) {
            if (key == '\r' || key == '\n') {
                if (cmd_idx > 0) {
                    cmd_buf[cmd_idx] = '\0'; 
                    if (strncasecmp(cmd_buf, "set ", 4) == 0) set_point = atof(cmd_buf + 4);
                    cmd_idx = 0;
                }
            } else if (key >= 32 && key <= 126 && cmd_idx < 15) {
                cmd_buf[cmd_idx++] = key;
            }
        }
    }

    // Rulare la fiecare 2 secunde
    if (t_curent - timp_anterior_dht >= 2000) {
        timp_anterior_dht = t_curent;
        float temp_noua = dd_dht_read_temperature();
        if (temp_noua != -999.0) {
            temp_curenta = temp_noua;
            executa_pid(t_curent);
            afiseaza_date();
        }
    }
}