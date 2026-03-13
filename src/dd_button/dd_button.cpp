#include "dd_button.h"
#include <Arduino.h>

#define BUTTON_PIN 2

void DD_Button_Init(void)
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

uint8_t DD_Button_Read(void)
{
    return (digitalRead(BUTTON_PIN) == LOW);
}