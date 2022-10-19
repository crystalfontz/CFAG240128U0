// Crystalfontz America, Inc. https://www.crystalfontz.com
// 2022-09-12 Kelsey Zaches 
//
//This code demonstrates the CFAG240128U0-*FH displays.
//These display:
//    *expect 3.3v logic, we use a seeeduino 4.2: https://www.crystalfontz.com/product/cfapn15062
//    *use an ST75256 controller. Information about the commands can be found in the controller datasheet.
//     https://www.crystalfontz.com/controllers/Sitronix/ST75256
//    *more info on product pages
//      https://www.crystalfontz.com/product/cfag240128u0nfh (no backlight)
//      https://www.crystalfontz.com/product/cfag240128u0tfh (yes backlight)
//
//============================================================================
// LCD SPI & control lines
//  seeeduino | Port | LCD
// -----------+------+-------------------------
//  #8/D8     |  PB0 | LCD_DC Data/Command or Register Select
//  #9/D9     |  PB1 | LCD_RESET
// #10/D10    |  PB2 | LCD_CS_NOT   (or SPI SS)
// #11/D11    |  PB3 | LCD_MOSI/DB1 (hardware SPI)
// #13/D13    |  PB5 | LCD_SCK/DB0  (hardware SPI)
//
// You will also need to tie the interface pins (IF0 and IF1) low for SPI
// see datasheet for other interfaces
//
//===========================================================================
//  This is free and unencumbered software released into the public domain.
//
//  Anyone is free to copy, modify, publish, use, compile, sell, or
//  distribute this software, either in source code form or as a compiled
//  binary, for any purpose, commercial or non-commercial, and by any
//  means.
//
//  In jurisdictions that recognize copyright laws, the author or authors
//  of this software dedicate any and all copyright interest in the
//  software to the public domain. We make this dedication for the benefit
//  of the public at large and to the detriment of our heirs and
//  successors. We intend this dedication to be an overt act of
//  relinquishment in perpetuity of all present and future rights to this
//  software under copyright law.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//
//  For more information, please refer to <http://unlicense.org/>
//===========================================================================


#include <Arduino.h>
#include <SPI.h>
#include "images.h"

#define CLR_DC    (PORTB &= ~(0x01))
#define SET_DC    (PORTB |=  (0x01))
#define CLR_RESET (PORTB &= ~(0x02))
#define SET_RESET (PORTB |=  (0x02))
#define CLR_CS    (PORTB &= ~(0x04))
#define SET_CS    (PORTB |=  (0x04))
#define CLR_MOSI  (PORTB &= ~(0x08))
#define SET_MOSI  (PORTB |=  (0x08))
#define CLR_SCK   (PORTB &= ~(0x20))
#define SET_SCK   (PORTB |=  (0x20))


#define PIN_TIME   (7)
#define CLR_TIME   (PORTD &= ~(0x80))
#define SET_TIME   (PORTD |=  (0x80))


#define HRES 240
#define VRES 128

uint8_t Vop = 0x2A; //15.5v - changing this will change the display contrast



void SPI_sendCommand(uint8_t command)
  {
  // Select the LCD's command register
  CLR_DC;
  // Select the LCD controller
  CLR_CS;
  //Send the command via SPI:
  SPI.transfer(command);
  // Deselect the LCD controller
  SET_CS;
  }
//============================================================================
void SPI_sendData(uint8_t data)
  {
  // Select the LCD's data register
  SET_DC;
  // Select the LCD controller
  CLR_CS;
  //Send the data via SPI:
  SPI.transfer(data);
  // Deselect the LCD controller
  SET_CS;
  }
//============================================================================


//============================================================================

void fill_display(uint8_t x1)
{
  SPI_sendCommand(0x30);
  SPI_sendCommand(0xf0);  //display mode
  SPI_sendData(0x10);     //0x11 - 4 gray scale mode, 0x10 bw
  SPI_sendCommand(0x5C);   // write data to ddram
  

  uint8_t i,j,k=HRES;
  

  for(i=0;i<k;i++)
    {
    for(j=0;j<VRES;j++)
      {
      //Send the data via SPI:
      SPI_sendData(x1);
      }
    }
}


void monoImage(const char image[16][240])
{


  SPI_sendCommand(0x30);  //extension command 1
  SPI_sendCommand(0xf0);  //display mode
  SPI_sendData(0x10);     //0x11 - 4 gray scale mode, 0x10 bw
	SPI_sendCommand(0x5c);

	uint8_t i,j;
	uint8_t k;
	k=VRES/8;  

	for(i=0;i<k;i++)
  {
    for(j=0;j<HRES;j++)
    {
      SPI_sendData(pgm_read_byte(&image[i][j]));
    }
  }
}



void GrayImage2Bit(const char image[32][240])
{
	SPI_sendCommand(0x30);
	SPI_sendCommand(0x81);// VOP SET
  SPI_sendData(Vop);
  SPI_sendData(0x04);	
	
	SPI_sendCommand(0x31);
  SPI_sendCommand(0x20);//SET GRAY LEVEL (#31)
  SPI_sendData(0x00);	
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x0b);//0x07
  SPI_sendData(0x0b);
  SPI_sendData(0x0b);
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x0f);//0x0d
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x0f);//0x0d
  SPI_sendData(0x0f);
  SPI_sendData(0x0f);
  SPI_sendData(0x00);
  SPI_sendData(0x00);


  SPI_sendCommand(0x40);
  SPI_sendCommand(0x30);

	SPI_sendCommand(0x75); //set page address
	SPI_sendData(0x00); //start address
	SPI_sendData(0x28);	//end address
	SPI_sendCommand(0xf0);
	SPI_sendData(0x11);	 //grayscale mode
	SPI_sendCommand(0x5c); //write data

  uint8_t i,j;

	for(i=0;i<32;i++)
  {
    for(j=0;j<240;j++)
    {
      SPI_sendData(pgm_read_byte(&image[i][j]));
    }
    
  }
}


//=============================================================

void Initialize_CFAG240128U(void)
{
  //Thump the hardware reset line.
  _delay_ms(1);
  CLR_RESET;
  _delay_ms(1);
  SET_RESET;
  _delay_ms(10);
  
  SPI_sendCommand(0x30);  //Extension command 1
  SPI_sendCommand(0x6E); //set commonn scan  - enable master
  SPI_sendCommand(0x31); //set lcd bias1/9 - extension command 2
  SPI_sendCommand(0xd7); //disable auto  read
  SPI_sendData(0x9f); 

  SPI_sendCommand(0xe0); //enable OTP read
  SPI_sendData(0x00);
  delay(1);

  SPI_sendCommand(0xe3); // OTP Up-Load
  delay(2); 

  SPI_sendCommand(0xe1); // OTP Control Out
  SPI_sendCommand(0x30); // Extension command 1
  SPI_sendCommand(0x94); // Sleep Out
  SPI_sendCommand(0xae); //#2. Display off
  delay(5);

  SPI_sendCommand(0x20); //power control
  SPI_sendData(0x0b); //VB,VR,VF all on

  SPI_sendCommand(0x81); //Set Vop
  SPI_sendData(Vop); //15.5v
  SPI_sendData(0x04); 

  SPI_sendCommand(0x31); // extension command 2 
  SPI_sendCommand(0x20); // set grayscale level
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x17);
  SPI_sendData(0x17);
  SPI_sendData(0x17);
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x1d);
  SPI_sendData(0x00);
  SPI_sendData(0x00);
  SPI_sendData(0x1d);
  SPI_sendData(0x1d);
  SPI_sendData(0x1d);
  SPI_sendData(0x00);
  SPI_sendData(0x00);

  SPI_sendCommand(0x32); //analog circuit set
  SPI_sendData(0x00);
  SPI_sendData(0x01);  //booster efficiency
  SPI_sendData(0x02); //bias =1/12

  SPI_sendCommand(0x51);  //Booster level x10
  SPI_sendData(0xfb); 

  SPI_sendCommand(0xf0); //frame rate
  SPI_sendData(0x02); //
  SPI_sendData(0x08);
  SPI_sendData(0x0f);
  SPI_sendData(0x18);

  SPI_sendCommand(0x30);
  SPI_sendCommand(0xf0);  //display mode
  SPI_sendData(0x10);     //0x11 - 4 gray scale mode, 0x10 bw

  SPI_sendCommand(0xca);  //display control
  SPI_sendData(0x00);     //cl dividing ratio
  SPI_sendData(0x7f);     //duty set - 64
  SPI_sendData(0x00);     // frame inversion (none)

  SPI_sendCommand(0xbc);  //data scan direction
  SPI_sendData(0x00);

  SPI_sendCommand(0xa6);  //normal display
  SPI_sendCommand(0x31);  //extension command 2
  SPI_sendCommand(0x40);  //internal power supply

  SPI_sendCommand(0x30);  //extension command 1
  SPI_sendCommand(0x15);  //column address setting
  SPI_sendData(0x00);     //starting column
  SPI_sendData(0xef);     //ending column
  SPI_sendCommand(0x75);  // page address setting
  SPI_sendData(0x00);     //starting page
  SPI_sendData(0x10);     //ending page
  SPI_sendCommand(0x30);  //extension command 1
  SPI_sendCommand(0xaf);  //#2. Display On
}

//================================================================================
void setup( void )
  {
  DDRB |= 0x2F;

  //Drive the ports to a reasonable starting state.
  SET_MOSI;
  SET_SCK;
  CLR_RESET;
  CLR_DC;
  SET_CS;
  CLR_MOSI;
  CLR_SCK;


  pinMode(PIN_TIME, OUTPUT);

  //debug console
  Serial.begin(115200);
  Serial.println(F("setup()"));
  // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  //Bump the clock to 8MHz. Appears to be the maximum.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));


  Initialize_CFAG240128U();
}

//================================================================================

void loop(void)
{
  Serial.println(F("loop()"));
  //Push out a few filled screens
  fill_display(0xFF);
  delay(1000);  
  fill_display(0x00);
  delay(1000);
  //Show splash screen LogoNFH for no backlight LogoTFH for backlight
  monoImage(LogoNFH);
  delay(2000);
  //Show grayscale picture of Mooch the dog
  GrayImage2Bit(mooch2bit);
  delay(2000);
  monoImage(LogoTFH);
  delay(2000);
}
