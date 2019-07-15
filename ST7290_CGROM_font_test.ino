// ST7920 CGROM built-in fonts test
// Code for the video:
// https://youtu.be/OyUXVsbAcmo

/*
 128x64 ST7920 connections (SPI)
 
 #01 GND  -> GND
 #02 VCC  -> VCC (5V)
 #04 RS   -> D10/CS or any pin
 #05 R/W  -> D11/MOSI
 #06 E    -> D13/SCK
 #15 PSB  -> GND (for SPI mode)
 #19 BLA  -> VCC or any pin via 300ohm resistor
 #20 BLK  -> GND

*/

#include <SPI.h>

#define LCD_RST    8  // optional, not necessary
#define LCD_CS     10

#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98

void sendCmd(byte b) 
{
  SPI.beginTransaction(SPISettings(200000UL, MSBFIRST, SPI_MODE3));
  digitalWrite(LCD_CS, HIGH);
  SPI.transfer(0xF8);
  SPI.transfer(b & 0xF0);
  SPI.transfer(b << 4);
  digitalWrite(LCD_CS, LOW);
  SPI.endTransaction();
}

void sendData(byte b) 
{
  SPI.beginTransaction(SPISettings(200000UL, MSBFIRST, SPI_MODE3));
  digitalWrite(LCD_CS, HIGH);
  SPI.transfer(0xFA);
  SPI.transfer(b & 0xF0);
  SPI.transfer(b << 4);
  digitalWrite(LCD_CS, LOW);
  SPI.endTransaction();
}

void LCD_init() 
{
  //digitalWrite(LCD_RST, LOW);
  //delay(100);
  //digitalWrite(LCD_RST, HIGH);

  sendCmd(LCD_BASIC);
  sendCmd(LCD_BASIC);
  sendCmd(LCD_CLS); delay(2);
  sendCmd(LCD_ADDRINC);
  sendCmd(LCD_DISPLAYON);
}

void setup() 
{
  //pinMode(LCD_RST, OUTPUT);
  SPI.begin();
  pinMode(LCD_CS, OUTPUT);
  digitalWrite(LCD_CS, LOW);

  LCD_init();
}

char buf[20];

void test8x16()
{
  int del = 2000;
  sendCmd(LCD_BASIC);
  unsigned int i,j,c=0;
  for(i=0;i<0x80;i++) {
    c++;
    if(c==16) sendCmd(LCD_LINE2);
    else if(c==32) sendCmd(LCD_LINE3);
    else if(c>=48 || i==0) {
      if(c>=48) delay(del);
      snprintf(buf,20,"%02X..%02X",i,i+47);
      sendCmd(LCD_CLS); delay(2);
      for(j=0;j<strlen(buf);j++) sendData(buf[j]);
      c=0;
      sendCmd(LCD_LINE1);
    }
    sendData(i==0?' ':i);
  }
  delay(del);
}

void test16x16()
{
  int del = 300;
  sendCmd(LCD_BASIC);
  unsigned int i,j,c=0;
  int st=0xa140;
  for(i=st;i<0xffff;i++) {
    c++;
    if(c==8) sendCmd(LCD_LINE2);
    else if(c==16) sendCmd(LCD_LINE3);
    else if(c>=24 || i==st) {
      if(c>=24) delay(i<0xac00?800:del);
      snprintf(buf,20,"%04X..%04X",i,i+23);
      sendCmd(LCD_CLS); delay(2);
      for(j=0;j<strlen(buf);j++) sendData(buf[j]);
      c=0;
      sendCmd(LCD_LINE1);
    }
    sendData(i>>8);
    sendData(i&0xff);
  }
  delay(del);
}

void loop() 
{
  test8x16();
  test16x16();
}

