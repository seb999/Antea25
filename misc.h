#ifndef MISC_H
#define	MISC_H
//Buzzer Rev001
//#define BUZZER PORTCbits.RC1
//Buzzer Rev002
#define BUZZER PORTCbits.RC0
#define LED PORTCbits.RC2

//  delay in second
void __delay_sec(int n){
    while(n--) __delay_ms(1000); 
//    for(int i=0;i<n;i++){
//        __delay_ms(1000);
   // }
}

//void Bip(int n, int t){
//    while(n--){
//        while(t--){
//            BUZZER = 1;
//            __delay_us(130);
//            BUZZER = 0;
//            __delay_us(130);
//        }
//        __delay_ms(20);
//    }
//}

void Bip(int n, int t){
    for(int i=0;i<n;i++){
        for (int j=0;j<t;j++){
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
    WDTCON = 0x18;//00011001  reset every 4 second
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
    
    WPUA = 0x00;
    WPUB = 0x00;
    WPUC = 0x00;
    OPTION_REGbits.nWPUEN = 0x01; //if 0 then individual weak pull up can be set

    bool state = GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS    
    
    SSPDATPPSbits.SSPDATPPS = 0x05; // RA5->MSSP:SDI
    RXPPSbits.RXPPS = 0x0C; // RB4->EUSART:RX
    RB5PPSbits.RB5PPS = 0x14; // RB5->EUSART:TX
    RC3PPSbits.RC3PPS = 0x12; // RC3->MSSP:SDO
    SSPCLKPPSbits.SSPCLKPPS = 0x16; // RC6->MSSP:SCK
    RC6PPSbits.RC6PPS = 0x10; // RC6->MSSP:SCK
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    GIE = state;
}

#endif	