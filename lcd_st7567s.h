/******************************
 * original code from
 * URL: https://github.com/mworkfun/ST7567A_128X32DOT_LCD
 * 
 *  
 * Modify for LCD ST7567S 128x64
 * screen:
 * x-----------------------> +
 * y                    |
 * |                    |
 * |---------------------
 * | 
 * v +
 *  
 ******************************/
#ifndef LCD_ST7567S_h
#define LCD_ST7567S_h

#include <Arduino.h>
#include <Wire.h>

#define DRAW_UPPER_RIGHT 0x01
#define DRAW_UPPER_LEFT  0x02
#define DRAW_LOWER_LEFT 0x04
#define DRAW_LOWER_RIGHT  0x08
#define DRAW_ALL (DRAW_UPPER_RIGHT|DRAW_UPPER_LEFT|DRAW_LOWER_RIGHT|DRAW_LOWER_LEFT)

class lcd_st7567s{
  public:
    // LCD is driven by ST7567A IC.
    // Define instructions on page 37 of the ST7567A data sheet.
    enum ST7567A_C {
      RMW        = 0xe0,  //Read-modify-Write command on page 42 of ST7567A datasheet.
      END        = 0xee,
    };
    
    lcd_st7567s();
    void WriteByte_command(int dat);      //Write a command to LCD.
    void WriteCont_command(int str[]);    //Continuously write commands to LCD.
    void Init();                          //This initialization function is called when using LCD.
    void testPixel(int t);                //use to test LCD screen.
    
    void WriteByte_dat(int dat);          //Write a data to RAM.
    void WriteCont_dat(int str[]);        //Continuously write data to RAM.
    int  ReadByte_dat(int col, int page); //read one byte RAM data to MCU. col=0-127, page=0-3
    void DisplayPixel(int x,int y);       //display one pixel. X=0-31, Y=0-127
    void ClearPixel(int x,int y);         //Does not display a pixel. X=0-31, Y=0-127 
    void Clear(bool invcolor);          // false = RAM write '0', all pixels turn off. 
  private:
    int addr = 0x3f;  //Address of LCD device.
};

#endif
