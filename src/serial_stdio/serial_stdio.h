#ifndef SERIAL_STDIO_H
#define SERIAL_STDIO_H
#include <Arduino.h>
#include <stdint.h>
void serial_stdio_setup();
int serial_put_char(char ch, FILE *stream);
int serial_get_char(FILE *stream);

#endif