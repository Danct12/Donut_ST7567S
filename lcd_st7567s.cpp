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
#include "lcd_st7567s.h"
#include <Arduino.h>
#include <Wire.h>

/******************************************
*******************************************/
lcd_st7567s::lcd_st7567s(){
  
}

/******************************************
 * Write a command to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd_st7567s::WriteByte_command(int dat){
  Wire.beginTransmission(addr);      // transmit to device 0x3f
  Wire.write(0x00);                  // Co=0,A0=0. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(dat);                   // sends restart command. 
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * Init the LCD.
 * This initialization function is called when using LCD.
*******************************************/
void lcd_st7567s::Init(){
  Wire.begin();
  delay(10);

  WriteByte_command(0xE2);    //Internal reset
  delay(10);
  WriteByte_command(0xA2);    //0xa2 LCD bias ratio is 1/9; 0xa3 is bias ratio 1/7; 128X64=1/9
  
  //// normal screen
  WriteByte_command(0xA0);    //(0xa0-0xa1 is ADC direction selection) 0xa0 is segment forward, 0xa1 is segment reverse
  WriteByte_command(0xC8);    //com output mode drive direction is reverse
	//// flip screen
  //WriteByte_command(0xA1);    // may be bug, loss first 4 bits in each row
  //WriteByte_command(0xC0);
  
  //WriteByte_command(0xA7);    // 0xA6 Normal, 0xA7 Inverse


  //********Adjust display brightness********
  WriteByte_command(0x25);    //0x20-0x27 is the internal Rb/Ra resistance adjustment setting of V5 voltage RR=4.5V
  //WriteByte_command(0x24);    // only 0x25 and 0x24 is ok 
  WriteByte_command(0x81);    //Electricity Mode Settings
  WriteByte_command(0x20);
  
  WriteByte_command(0x2C);    //Internal Power Supply Control Mode
  WriteByte_command(0x2E);
  WriteByte_command(0x2F);
  Clear(false);                        //initalize DDRAM

  WriteByte_command(0xAF);    
  WriteByte_command(0x40);

}

/******************************************
 * Used to test screen pixels.
*******************************************/
void lcd_st7567s::testPixel(int t){
  for(int x=0; x<8; x++){
    WriteByte_command(0xb0 + x);  //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows

    /*******automatically increased by one******/
    WriteByte_command(0x10);      //x, column address x=0-0-0-0-1-x7-x6-x5-x4
    WriteByte_command(0x00);      //x, column address x=0-0-0-0-0-x3-x2-x1-x0
    
    for(int i=0; i<128; i++){
      WriteByte_dat(0xff);        //row=bit0--bit7, display on
      delay(t);
      }  
  }
  for(int x=0; x<8; x++){
    WriteByte_command(0xb0 + x);
    WriteByte_command(0x00);
    WriteByte_command(0x10);
    for(int i=0; i<128; i++){
      WriteByte_dat(0x00);       //row=bit0--bit7, display off
      delay(t);
      }  
  }
}

/******************************************
 * Write a data to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd_st7567s::WriteByte_dat(int dat){
  Wire.beginTransmission(addr);      // transmit to device 0x7e
  Wire.write(0x40);                  // Co=0,A0=1. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(dat);                   // sends data. 
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * Continuously write data to LCD.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
void lcd_st7567s::WriteCont_dat(int str[]){
  int len = 0;
  len = sizeof(str);
  Wire.beginTransmission(addr);      // transmit to device 0x7e
  for(int i=0; i<len-1; i++){
    Wire.write(0xc0);                // Co=1,A0=1. data= Co-A0-0-0-0-0-0-0. 
    Wire.write(str[i]);              // sends command.
  }
  Wire.write(0x40);                  // Co=0,A0=1. data= Co-A0-0-0-0-0-0-0. 
  Wire.write(str[len-1]);            // sends command.
  Wire.endTransmission();            // stop transmitting
}

/******************************************
 * read one byte RAM data to MCU.
 * Define instructions on page 23 of the ST7567A data sheet.
*******************************************/
int lcd_st7567s::ReadByte_dat(int col, int page){
  int dat;
  WriteByte_command(0xb0 + page);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + col/16);   //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(col%16);          //x, column address x=0-0-0-0-0-x3-x2-x1-x0 

  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  return dat;
}

/******************************************
 * display one pixel. Read-modify-Write command on page 42 of ST7567A datasheet.
 * x=0-128, y=0-31
*******************************************/
void lcd_st7567s::DisplayPixel(int x,int y){
  int dat;
  WriteByte_command(0xb0 + y/8);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + x/16);    //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(x%16);           //x, column address x=0-0-0-0-0-x3-x2-x1-x0 
  
  WriteByte_command(RMW);            //start Read-modify-Write
  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  dat = (dat | (1<<(y%8)));
  WriteByte_dat(dat);                //row=bit0--bit7
  WriteByte_command(END);            //end Read-modify-Write
}

/******************************************
 * Does not display a pixel. Read-modify-Write command on page 42 of ST7567A datasheet.
 * x=0-128, y=0-31
*******************************************/
void lcd_st7567s::ClearPixel(int x,int y){
  int dat;
  WriteByte_command(0xb0 + y/8);     //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows 
  WriteByte_command(0x10 + x/16);    //x, column address x=0-0-0-0-1-x7-x6-x5-x4
  WriteByte_command(x%16);           //x, column address x=0-0-0-0-0-x3-x2-x1-x0 
  
  WriteByte_command(RMW);            //start Read-modify-Write
  for(int i=0; i<2; i++){
    Wire.requestFrom(addr, 2);       //request 2 bytes from slave device
    while(Wire.available()){         //slave may send less than requested
      dat = Wire.read();             //receive a byte as character
    }
  }
  //Serial.println(dat);             // print the data
  //dat = (dat & (0xfe<<(y%8)));
  dat &= ~(1<<(y%8));
  WriteByte_dat(dat);                //row=bit0--bit7
  WriteByte_command(END);            //end Read-modify-Write 
}

/******************************************
 * clear screen, all pixel off.
 * screen size: 128*32 dot
*******************************************/
void lcd_st7567s::Clear(bool invcolor){
  for(int x=0; x<8; x++){
    WriteByte_command(0xb0 + x);   //y, page address y=1-0-1-1-y3-y2-y1-y0, 1-page with 8-rows
    
    /*******automatically increased by one******/
    WriteByte_command(0x10);       //x, column address x=0-0-0-0-1-x7-x6-x5-x4
    WriteByte_command(0x00);       //x, column address x=0-0-0-0-0-x3-x2-x1-x0
    
    for(int i=0; i<128; i++){
      if (invcolor) {
        WriteByte_dat(0xff);         //row=bit0--bit7
      }
      else {
        WriteByte_dat(0x00);         //row=bit0--bit7
      }
    }  
  }
}
