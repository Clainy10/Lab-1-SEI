#include "dd_button.h"
#include <Arduino.h>

#define BUTTON_PIN 2

void dd_button_init(void)
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

uint8_t dd_button_read(void)
{
    return (digitalRead(BUTTON_PIN) == LOW);
}