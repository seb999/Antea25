#ifndef MISC_H
#define	MISC_H
#define BUZZER  PORTCbits.RC0
#define LED     PORTCbits.RC2
#define SW1     PORTBbits.RB7
#define SW2     PORTBbits.RB6

//  delay in second
void __delay_sec(int n){
    while(n--) __delay_ms(1000); 
}

void Bip(int n){
    for(int i=0;i<n;i++){
        for (int j=0;j<100;j++){
                BUZZER = 1;
                __delay_us(130);
                BUZZER = 0;
                __delay_us(130);
        }
        __delay_ms(20);
    }
    BUZZER = 1;
}

void OSC_Init(void) {
    // SPLLEN disabled; SCS INTOSC; IRCF 8MHz_HF;
    OSCCON = 0x78;
    // OSTS intosc; HFIOFR disabled; SOSCR disabled; HFIOFS not0.5percent_acc; PLLR disabled; MFIOFR disabled; HFIOFL not2percent_acc; LFIOFR disabled;
    OSCSTAT = 0x00;
    // TUN 0x0;
    OSCTUNE = 0x00;
    //Watchdog
    WDTCON = 0x1B;//00011011  reset every 8 second
}

void PIN_Init(void) { 
    
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    
    TRISA = 0x23;
    TRISB = 0xF0;
    TRISC = 0x02;
    
    ANSELA = 0x03;
    ANSELB = 0x00;
    ANSELC = 0x00;
    
    //Swith with resistor REV2
//    WPUA = 0x00;
//    WPUB = 0x00;
//    WPUC = 0x00;
//    OPTION_REGbits.nWPUEN = 0x01; //if 0 then individual weak pull up can be set
    
    //Switch weak pull up REV3
    WPUA = 0x00;
    WPUC = 0x00;
    WPUB6 = 1;
    WPUB7 = 1;
    OPTION_REGbits.nWPUEN = 0x00;

    bool state = GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS    
    
    SSPDATPPSbits.SSPDATPPS = 0x05; // RA5->MSSP:SDI
    RC3PPSbits.RC3PPS = 0x12; // RC3->MSSP:SDO
    SSPCLKPPSbits.SSPCLKPPS = 0x16; // RC6->MSSP:SCK
    RC6PPSbits.RC6PPS = 0x10; // RC6->MSSP:SCK
    
    RXPPSbits.RXPPS = 0x0C; // RB4->EUSART:RX
    RB5PPSbits.RB5PPS = 0x14; // RB5->EUSART:TX
    
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
}

void INTER_Init(void){
    TMR0IE = 1; //enable TMR0 overflow interrupts
    //GIE = 1; //enable Global interrup   
    TMR0CS=0; //Internal instruction cycle clock  
    TMR0SE=0;
    PSA=0; 
    PS0=1; 
    PS1=1; 
    PS2=1; 
    TMR0 = 0; //clear timer
    
    //Activate Pin interruption
    IOCIE = 1;
    //Activate Pin interruption RC1
    IOCCP1 = 1;  
    //Activate Pin interruption RB7 (SW1)
    IOCBN7 = 1;
    //Activate Pin interruption RB6 (SW2)
    IOCBN6 = 1;
}

#endif	