/* Copyright 2024 Danct12
 * 
 * donut.c originally written by Andy Sloane.
 * 
 * Check out their writeup here:
 * https://www.a1k0n.net/2011/07/20/donut-math.html
 * 
 * This is a quick port of the donut.c code to Arduino, it requires a ST7567S display.
 * It's tested and working on RP2040, but it should work with ESP8266, ESP32, etc.
 */

#include "lcd_st7567s.h"

const int SCR_WIDTH = 128;
const int SCR_HEIGHT = 64;

int x_start=0, y_start=0;

int k;
double sin(),cos();

float A=0, B=0, i, j, z[SCR_WIDTH*SCR_HEIGHT];
char b[SCR_WIDTH*SCR_HEIGHT];

lcd_st7567s Lcd;

void setup() {
  Wire.setClock(400000); // Make display a bit faster
  Lcd.Init();
}

void loop() {
    memset(b,32,sizeof(b));
    memset(z,0,sizeof(z));
    for(j=0; 6.28>j; j+=0.07) {
        for(i=0; 6.28 >i; i+=0.02) {
            float sini=sin(i),
                  cosj=cos(j),
                  sinA=sin(A),
                  sinj=sin(j),
                  cosA=cos(A),
                  cosj2=cosj+2,
                  mess=1/(sini*cosj2*sinA+sinj*cosA+5),
                  cosi=cos(i),
                  cosB=cos(B),
                  sinB=sin(B),
                  t=sini*cosj2*cosA-sinj* sinA;
            int x=(SCR_WIDTH/2)+60*mess*(cosi*cosj2*cosB-t*sinB),
                y=(SCR_HEIGHT/2)+30*mess*(cosi*cosj2*sinB +t*cosB),
                o=x+SCR_WIDTH*y,
                N=8*((sinj*sinA-sini*cosj*cosA)*cosB-sini*cosj*sinA-sinj*cosA-cosi *cosj*sinB);
            if(SCR_HEIGHT>y&&y>0&&x>0&&SCR_WIDTH>x&&mess>z[o]){
                z[o]=mess;
                b[o]=N>0?N:0;
            }
        }
    }

    if (y_start > SCR_HEIGHT - 1)
      y_start = 0;

    Lcd.Clear(false);

    for(k=0; sizeof(b)+1>k; k++) {
        if (k == 0 || k % SCR_WIDTH) {
          if (b[k] != 32) {
            Lcd.DisplayPixel(x_start, y_start);
          }
          x_start++;
        } else {
          y_start++;
          x_start = 0;
          if (b[k] != 32) {
            Lcd.DisplayPixel(x_start, y_start);
          }
          x_start += 1;
        }
    }
    A+=0.04;
    B+= 0.02;
}
