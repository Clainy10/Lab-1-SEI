#include "serial_stdio/serial_stdio.h"
#include <Arduino.h>
#include <stdio.h>

void serial_stdio_setup()
{
    Serial.begin(9600);
    FILE *serial_stream = fdevopen(serial_put_char, serial_get_char);
    stdin = serial_stream;
    stdout = serial_stream;
}
    
int serial_put_char(char ch, FILE *stream)
{
   return Serial.write(ch);
}

int serial_get_char(FILE *stream)
{
    while (!Serial.available())
        ;
    return Serial.read();
}
