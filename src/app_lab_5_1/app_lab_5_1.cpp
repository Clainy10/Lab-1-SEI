#include "app_lab_5_1.h"
#include "dd_dht/dd_dht.h"
#include "dd_relay/dd_relay.h"
#include "serial_stdio/serial_stdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#define BUF_SIZE 16

static float temp_curenta = 0.0;
static float set_point = 24.0;   
static float histereza = 2.0f;    

static unsigned long timp_anterior_dht = 0;

static char cmd_buffer[BUF_SIZE];
static int cmd_idx = 0;

static void aplica_control_histereza(void) {
    if (temp_curenta == -999.0) return; 

    float prag_superior = set_point + histereza;
    float prag_inferior = set_point - histereza;

    if (temp_curenta >= prag_superior) {
        dd_relay_off();
    } 
    else if (temp_curenta <= prag_inferior) {
        dd_relay_on();
    }
}

static void afiseaza_status_sistem(void) {
    printf("\r\n=== MONITORIZARE TERMICA ===\r\n");
    printf("Temperatura Curenta: ");
    Serial.print(temp_curenta, 1);
    printf(" C\r\nSet Point curent     : ");
    Serial.print(set_point, 1);
    printf(" C\r\nHistereza            : +/- ");
    Serial.print(histereza, 1);
    printf(" C\r\nStare Releu/Incalzire: %s\r\n", dd_relay_is_on() ? "ON" : "OFF");
    printf("============================\r\n");
    printf("Introdu comanda (ex: set 25): ");
}

void app_lab_5_1_setup(void) {
    serial_stdio_setup(); 
    dd_relay_setup();      
    dd_dht_setup();  
    delay(2000); 
    
    cmd_idx = 0;
    memset(cmd_buffer, 0, BUF_SIZE);
    
    temp_curenta = dd_dht_read_temperature();
    afiseaza_status_sistem();
}

void app_lab_5_1_run(void) {
    unsigned long timp_curent = millis();

    if (Serial.available() > 0) {
        char key;
        if (scanf("%c", &key) == 1) {
            if (key == '\r' || key == '\n') {
                if (cmd_idx > 0) {
                    cmd_buffer[cmd_idx] = '\0'; 
                    
                    if (strncasecmp(cmd_buffer, "set ", 4) == 0) {
                        set_point = atof(cmd_buffer + 4);
                        printf("\r\n[OK] Set Point actualizat!\r\n");
                    } else {
                        printf("\r\n[EROARE] Comanda invalida: %s\r\n", cmd_buffer);
                    }
                    
                    cmd_idx = 0;
                    afiseaza_status_sistem();
                }
            } 
            else if (key >= 32 && key <= 126 && cmd_idx < (BUF_SIZE - 1)) {
                cmd_buffer[cmd_idx++] = key;
                printf("%c", key);
            }
        }
    }

    if (timp_curent - timp_anterior_dht >= 2000) {
        timp_anterior_dht = timp_curent;
        float temp_noua = dd_dht_read_temperature();
        
        if (temp_noua != -999.0) {
            temp_curenta = temp_noua;
            aplica_control_histereza();
            afiseaza_status_sistem();
        }else {
            printf("\r\n[EROARE] Citire temperatura esuata!\r\n");
        }
    }
}