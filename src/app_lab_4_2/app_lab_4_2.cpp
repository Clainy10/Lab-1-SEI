#include "dd_l298/dd_l298.h"
#include "lcd_stdio/lcd_stdio.h"
#include "keypad_stdio/keypad_stdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#define BUF_SIZE      32
#define CMD_CLEAR_LCD "#"
#define KEY_ENTER     '#'
#define KEY_RESET     '*'

static char cmd_buffer[BUF_SIZE];
static int cmd_idx = 0;

/**
 * Resetează starea buffer-ului text
 */
static void reset_input_state(void) {
    cmd_idx = 0;
    memset(cmd_buffer, 0, BUF_SIZE);
}

/**
 * Raportează starea curentă a motorului prin printf pe LCD/Serial
 */
static void report_system_status(void) {
    printf(CMD_CLEAR_LCD); // Curățăm ecranul folosind caracterul tău cheie
    int16_t pwr = dd_l298_get_power();

    if (pwr == 0) {
        printf("Motor: STOP\nPower: 0%%");
    } else {
        printf("Motor: %s\nPower: %d%%", (pwr > 0) ? "FORWARD" : "REVERSE", pwr);
    }
}

/**
 * Analizează string-ul primit și apelează funcția corectă din driver
 */
static void process_motor_command(void) {
    // 1. motor set [-100 .. 100]
    if (strncasecmp(cmd_buffer, "motor set ", 10) == 0) {
        int val = atoi(&cmd_buffer[10]);
        dd_l298_set_power(val);
    }
    // 2. motor stop
    else if (strcasecmp(cmd_buffer, "motor stop") == 0) {
        dd_l298_stop();
    }
    // 3. motor max
    else if (strcasecmp(cmd_buffer, "motor max") == 0) {
        dd_l298_max();
    }
    // 4. motor inc
    else if (strcasecmp(cmd_buffer, "motor inc") == 0) {
        dd_l298_inc();
    }
    // 5. motor dec
    else if (strcasecmp(cmd_buffer, "motor dec") == 0) {
        dd_l298_dec();
    }
    // Comandă eronată
    else {
        printf(CMD_CLEAR_LCD);
        printf("Err: %s\nInvalida", cmd_buffer);
        delay(1500); 
        report_system_status();
        return;
    }

    // Afișează noul status după rularea comenzii valide
    report_system_status();
}

/**
 * Configurația inițială a sistemului
 */
void app_lab_4_2_setup(void) {
    dd_l298_setup();
    lcd_stdio_setup();
    keypad_stdio_setup();
    
    report_system_status();
    reset_input_state();
}

/**
 * Bucla infinită de execuție non-blocking
 */
void app_lab_4_2_run(void) {
    char key;

    // Citire non-blocking prin arhitectura STDIO cu scanf
    if (scanf("%c", &key) != 1) {
        return; // Ieșim rapid dacă nu s-a apăsat nimic
    }

    // Dacă s-a apăsat tasta ENTER
    if (key == KEY_ENTER) {
        cmd_buffer[cmd_idx] = '\0'; // Închidem string-ul
        process_motor_command();
        reset_input_state();
    } 
    // Dacă s-a apăsat tasta de RESET
    else if (key == KEY_RESET) {
        reset_input_state();
        report_system_status();
    } 
    // Acumularea caracterelor normale în buffer
    else {
        if (cmd_idx < (BUF_SIZE - 1)) {
            cmd_buffer[cmd_idx++] = key;
            printf("%c", key); // Echo direct pe LCD/Serial în timp ce tastezi
        }
    }
}