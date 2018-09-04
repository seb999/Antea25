#ifndef OLED_H
#define	OLED_H

#include "font.h"

//  SPI Register
#define DOWN 0
#define UP 1
#define OLED_CS             PORTAbits.RA4
#define OLED_DC             PORTCbits.RC5
#define OLED_RES            PORTCbits.RC4

typedef char                     schar;
typedef unsigned char            uchar;
typedef const char               cschar;
typedef const unsigned char      cuchar;
typedef int                      sint;
typedef unsigned int             uint;

cuchar *font, *font2;
uchar width, height, min, max;

void Oled_WriteCmd( uchar cmd ){
    OLED_CS = DOWN;
    OLED_DC = DOWN;
    SPI_Write(cmd);
    OLED_CS = UP;
}

void Oled_WriteRam( uchar dat ){
    OLED_CS = DOWN;
    OLED_DC = UP;
    SPI_Write(dat);
    OLED_CS = UP;
}

//------------------------------------------------------------------------------
// Reset the OLED module.
//------------------------------------------------------------------------------
void Oled_Reset(void){
    OLED_RES = DOWN;
    __delay_ms(200);
    OLED_RES = UP;
    __delay_ms(200);
}

//------------------------------------------------------------------------------
// Set  pointer in frame memory where MCU can access.
// seg: set segment
// pag: set page
//------------------------------------------------------------------------------
void Oled_SetPointer(uchar seg, uchar pag){
    uchar low_column, hig_column;
    low_column = (seg & 0b00001111);
    hig_column = (seg & 0b11110000)>>4;
    hig_column = hig_column | 0b00010000;
    pag = (pag & 0b00000111);
    pag = (pag | 0b10110000);
    Oled_WriteCmd(low_column); // Set Lower Column
    Oled_WriteCmd(hig_column); // Set Higher Column
    Oled_WriteCmd(pag);        // Set Page Start
}

//------------------------------------------------------------------------------
// Fills OLED memory.
// pattern: byte to fill OLED memory
//------------------------------------------------------------------------------
void Oled_FillScreen(uchar pattern){
    unsigned char i,j;
    for(i = 0; i < 8; i++){
        Oled_SetPointer(0, i);
        for(j = 0; j < 128; j++){
            Oled_WriteRam(pattern);
        }
    }
}

//------------------------------------------------------------------------------
// Initializes the OLED module.
//------------------------------------------------------------------------------
void Oled_Init(void){
    OLED_CS = UP;
    OLED_DC = DOWN;
    OLED_RES = DOWN;
    __delay_ms(500);

    Oled_Reset();
    Oled_WriteCmd(0xAE);                     // Set Display OFF
    Oled_WriteCmd(0x81);
    Oled_WriteCmd(0xCF); // Set Contrast Control
    Oled_WriteCmd(0xA4);                     // Entire Display ON
    Oled_WriteCmd(0xA6);                     // Set Normal
    Oled_WriteCmd(0x20); Oled_WriteCmd(0x02); // Set Memory Addressing Mode
    Oled_WriteCmd(0x00);                     // Set Lower Column
    Oled_WriteCmd(0x10);                     // Set Higher Column
    Oled_WriteCmd(0xB0);                     // Set Page Start
    Oled_WriteCmd(0x40);                     // Set Display Start Line
    Oled_WriteCmd(0xA1);                     // Set Segment Re-map
    Oled_WriteCmd(0xA8); Oled_WriteCmd(0x3F); // Set Multiplex Ratio
    Oled_WriteCmd(0xC8);                     // Set COM Output
    Oled_WriteCmd(0xD3); Oled_WriteCmd(0x00); // Set Display Offset
    Oled_WriteCmd(0xDA); Oled_WriteCmd(0x12); // Set COM Pins Hardware Configuration
    Oled_WriteCmd(0xD5); Oled_WriteCmd(0x80); // Set Display Clock
    Oled_WriteCmd(0xD9); Oled_WriteCmd(0xF1); // Set Pre-charge Period
    Oled_WriteCmd(0xDB); Oled_WriteCmd(0x40); // Set VCOMH Deselect Level
    Oled_WriteCmd(0x8D); Oled_WriteCmd(0x14); // Charge Pump Setting
    Oled_WriteCmd(0xAF);                     // Set Display ON
    Oled_FillScreen(0x00);                  // Clear screen
}

void ScreenOn(){
    Oled_WriteCmd(0xAF);
}

void ScreenOff(){
    Oled_WriteCmd(0xAE); 
}

//------------------------------------------------------------------------------
// Sets font that will be used.
// _font: byte of array
// _width: char width (pixels)
// _height: char height (pixels)
// _min: range minimum
// _max: range maximum
//
// Note: only fonts with multiples of 8 pixels in height.
//------------------------------------------------------------------------------
void Oled_SetFont(cuchar *_font, uchar _width, uchar _height, uchar _min, uchar _max){
    font2  = _font;
    width  = _width;
    height = _height / 8;
    min    = _min;
    max    = _max;
}

//------------------------------------------------------------------------------
// Writes a char on the OLED at coordinates (x, y).
// c: char to be written
// seg: set segment. Valid values: 0..127
// pag: set page. Valid values: 0..7
//------------------------------------------------------------------------------
void Oled_WriteChar(uchar c, uchar seg, uchar pag){
    uint i,j;
    uchar x_seg, y_pag;
    x_seg = seg;
    y_pag = pag;
    font = font2;
    j = c - min;
    j = j * (width * height);
    for(i = 0; i < j; i++){font++;}
    for(i = 0; i < width; i++){
        y_pag = pag;
        for(j = 0; j < height; j++){
            if(x_seg < 128){
               Oled_SetPointer(x_seg, y_pag);
               Oled_WriteRam(*font);
            }
            y_pag++;
            font++;
        }
        x_seg++;
    }
}

//------------------------------------------------------------------------------
// Prints text  constant on OLED.
// buffer: byte of array
// seg: set segment. Valid values: 0..127
// pag: set page. Valid values: 0..7
//------------------------------------------------------------------------------
void Oled_ConstText(cschar *buffer, uchar seg, uchar pag){
    uchar x_seg = seg;
    while(*buffer){
        Oled_WriteChar(*buffer, x_seg, pag);
        x_seg = x_seg + width;
        buffer++;
    }
}
//------------------------------------------------------------------------------
// Extra routines for configuration and start the Scroll function.
// start_page: start page. Valid values: 0..7
// end_page: end page. Valid values: 0..7
// set_time: set time. Valid values: 0..7
//------------------------------------------------------------------------------
void Deactivate_Scroll(void){
    Oled_WriteCmd(0x2E);
}

void Activate_Scroll(void){
    Oled_WriteCmd(0x2F);
}

void Left_HorizontalScroll(uchar start_page, uchar end_page, uchar set_time){
    Deactivate_Scroll();
    Oled_WriteCmd(0x27);
    Oled_WriteCmd(0x00);
    Oled_WriteCmd(start_page);
    Oled_WriteCmd(set_time);
    Oled_WriteCmd(end_page);
    Oled_WriteCmd(0x00);
    Oled_WriteCmd(0xFF);
    Activate_Scroll();
}
#endif	/* OLED_H */