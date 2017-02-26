#ifndef SPI_H
#define	SPI_H

#define SPI_RX_IN_PROGRESS 0x0

void SPI_Init(void)
{
    // BF RCinprocess_TXcomplete; UA dontupdate; SMP Sample At Middle; P stopbit_notdetected; S startbit_notdetected; R_nW write_noTX; CKE Active to Idle; D_nA lastbyte_address;
    SSP1STAT = 0x40;
    //SSP1STAT = 0x00;
    //SSP1STAT = 0x80;
    // SSPEN enabled; WCOL no_collision; SSPOV no_overflow; CKP Idle:High, Active:Low; SSPM FOSC/4;
    SSP1CON1 = 0x30;
    //SSP1CON1 = 0x20;
    //SSP1CON1 = 0x21; __Low speed
    // SSP1ADD 0;
    SSP1ADD = 0x00;
}

uint8_t SPI_Write(uint8_t data)
{
    // Clear the Write Collision flag, to allow writing
    SSP1CON1bits.WCOL = 0;
    SSPBUF = data;
    while(SSP1STATbits.BF == SPI_RX_IN_PROGRESS)
    {
    }
    return (SSPBUF);
}

#endif

