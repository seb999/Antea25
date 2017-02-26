#ifndef UART_H
#define	UART_H
#include <stdint.h>

char UART_Init(void)
{
    // ABDEN disabled; WUE disabled; RCIDL idle; ABDOVF no_overflow; SCKP async_noninverted_sync_fallingedge; BRG16 16bit_generator;
    BAUD1CON = 0x48;
    // ADDEN disabled; RX9 8-bit; RX9D 0x0; FERR no_error; CREN enabled; SPEN enabled; SREN disabled; OERR no_error;
    RC1STA = 0x90;
    // CSRC slave_mode; TRMT TSR_empty; TXEN enabled; BRGH hi_speed; SYNC asynchronous; SENDB sync_break_complete; TX9D 0x0; TX9 8-bit;
    TX1STA = 0x26;

    //Rev001
    SP1BRGL = 0xCF;
    SP1BRGH = 0x00;
    
    //Rev002
    //SP1BRGL = 0xA0;
    //SP1BRGH = 0x01;
}

//--------------------UART TRANSMISSION----------------------------------------
void UART_Write(uint8_t data)
{
    while(0 == PIR1bits.TXIF);
    TX1REG = data;    // Write the data byte to the USART.
}

void UART_Write_Text(char *text)
{
  for(int i=0;text[i] !=  '\0' ;i++)
    UART_Write(text[i]);
}

void SendUartCmd (char * cmd){
    UART_Write_Text(cmd);
}
//--------------------UART RECEPTION----------------------------------------
char UART_Data_Ready()
{
  return RCIF;
}

uint8_t UART_Read(void)
{
    while(!PIR1bits.RCIF)
    if(1 == RC1STAbits.OERR)
    {
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
    }
    return RC1REG;
}

void UART_Read_Text(char *Output, unsigned int length)
{
    Output[0] = UART_Read();
    for(int i=1;i<length;i++)
    {
        Output[i] = UART_Read();
        if(Output[i-1]=='O' && Output[i]=='K')
            break;
    }
}

#endif

