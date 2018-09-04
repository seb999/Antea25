#ifndef ADXL_H
#define	ADXL_H

// ACC Registers
#define ADXL_RESET 0x1f
#define ADXL_INTMAP2 0x2b
#define ADXL_POWER_CTL 0x2d
#define ADXL_WR_SPI 0x0A
#define ADXL_RD_SPI 0x0B
#define DOWN 0
#define UP 1
#define ADXL_CS PORTAbits.RA2
#define ADXL_INT2 PORTCbits.RC1

//Read SPI bus
char ACC_ReadReg( char reg )
{
    ADXL_CS = DOWN;
    SPI_Write(ADXL_RD_SPI);
    SPI_Write(reg);
    CKP=0;
    char val = SPI_Write(0x00);
    ADXL_CS = UP;
    __delay_ms(100);
    return (val);
}

//Write SPI bus
void ACC_WriteReg( char reg, char cmd )
{
    ADXL_CS = DOWN;
    SPI_Write(ADXL_WR_SPI);
    SPI_Write(reg);
    SPI_Write(cmd);
    ADXL_CS = UP;
    __delay_ms(100);
}

void ACC_Init()
{
    ADXL_CS=UP;
    char reg;

    // reset the adxl362
    ACC_WriteReg(ADXL_RESET, 0x52);

    //Check Reset done
    reg = ACC_ReadReg(0x0b);
    reg = ACC_ReadReg(0x00);
    reg = ACC_ReadReg(0x2c);

   // Activity
    ACC_WriteReg(0x20,0xFA);
    ACC_WriteReg(0x21,0x0);
    ACC_WriteReg(0x22,0xA); //Time before going in awake mode (30 = 5 sec)
    
    // INACTIVITY
    ACC_WriteReg(0x23,0x96);
    ACC_WriteReg(0x24,0x0);
    ACC_WriteReg(0x25,0x1);//Time before going in sleep mode 
   
    //Put in loops mode
    ACC_WriteReg(0x27,0x3F); //in reference mode
    
    //awake mode
    ACC_WriteReg(0x2B,0x40);
    
    //STart reading
    ACC_WriteReg(0x2d,0x0A);
}

#endif	/* ADXL_H */

