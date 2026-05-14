#include "lcd_stdio.h" 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

char CLEAR_KEY[] = "#";
#define BACKLIGHT_PIN     20
#define lcdColumns        16
#define lcdRows           2

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Variabile globale pentru a urmări poziția cursorului
int curent_col = 0;
int curent_row = 0;

void lcd_stdio_setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  
  curent_col = 0;
  curent_row = 0;
  
  FILE* stream = fdevopen(lcd_stdio_print, NULL);
  stdout = stream;
}

int lcd_stdio_print(char c, FILE* stream)
{
  // 1. Tratarea tastei de ștergere
  if (c == CLEAR_KEY[0])
  {
    lcd.clear(); 
    lcd.setCursor(0, 0);
    curent_col = 0;
    curent_row = 0;
  }
  // 2. Tratarea caracterului de linie nouă '\n'
  else if (c == '\n')
  {
    curent_col = 0;                 // Resetăm coloana la începutul liniei
    curent_row = (curent_row + 1) % lcdRows; // Trecem la rândul următor (și revenim la 0 dacă depășim)
    lcd.setCursor(curent_col, curent_row);
  }
  // 3. Tratarea caracterelor normale
  else
  {
    lcd.print(c);
    curent_col++; // Incrementăm coloana după fiecare caracter printat
    
    // Auto-wrap: Dacă textul depășește lățimea ecranului, trecem automat la linie nouă
    if (curent_col >= lcdColumns)
    {
      curent_col = 0;
      curent_row = (curent_row + 1) % lcdRows;
      lcd.setCursor(curent_col, curent_row);
    }
  }
  
  return 0;
}